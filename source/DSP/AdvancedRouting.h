#pragma once

#include "../JuceHeader.h"
#include "AudioModule.h"
#include <vector>
#include <memory>

using namespace juce;

//==============================================================================
/**
 * Advanced routing graph for complex signal paths
 * 
 * Supports:
 * - Serial chains (A → B → C)
 * - Parallel paths (A → [B, C] → D)
 * - Send/Return loops
 * - Flexible routing
 */
class RoutingGraph
{
public:
    enum class RoutingMode
    {
        Serial,      // Simple serial chain (default)
        Parallel,    // Split into parallel paths
        SendReturn   // Send/return loop
    };
    
    struct RoutingNode
    {
        std::unique_ptr<AudioModule> module;
        RoutingMode mode = RoutingMode::Serial;
        std::vector<int> connections;  // Indices of connected nodes
        float mixLevel = 1.0f;         // For parallel/send mixing
        bool isSendReturn = false;
    };
    
    RoutingGraph() = default;
    ~RoutingGraph() = default;
    
    //==============================================================================
    // Node Management
    
    /** Adds a node to the graph */
    int addNode(std::unique_ptr<AudioModule> module)
    {
        RoutingNode node;
        node.module = std::move(module);
        nodes.push_back(std::move(node));
        return static_cast<int>(nodes.size()) - 1;
    }
    
    /** Connects two nodes */
    void connect(int fromNode, int toNode)
    {
        if (fromNode >= 0 && fromNode < (int)nodes.size())
        {
            auto& connections = nodes[fromNode].connections;
            if (std::find(connections.begin(), connections.end(), toNode) == connections.end())
            {
                connections.push_back(toNode);
            }
        }
    }
    
    /** Disconnects two nodes */
    void disconnect(int fromNode, int toNode)
    {
        if (fromNode >= 0 && fromNode < (int)nodes.size())
        {
            auto& connections = nodes[fromNode].connections;
            connections.erase(
                std::remove(connections.begin(), connections.end(), toNode),
                connections.end()
            );
        }
    }
    
    /** Sets routing mode for a node */
    void setRoutingMode(int nodeIndex, RoutingMode mode)
    {
        if (nodeIndex >= 0 && nodeIndex < (int)nodes.size())
        {
            nodes[nodeIndex].mode = mode;
        }
    }
    
    /** Sets mix level for a node (for parallel/send routing) */
    void setMixLevel(int nodeIndex, float level)
    {
        if (nodeIndex >= 0 && nodeIndex < (int)nodes.size())
        {
            nodes[nodeIndex].mixLevel = jlimit(0.0f, 1.0f, level);
        }
    }
    
    //==============================================================================
    // Processing
    
    /** Prepares all nodes */
    void prepare(double sampleRate, int maxBlockSize)
    {
        for (auto& node : nodes)
        {
            if (node.module != nullptr)
                node.module->prepare(sampleRate, maxBlockSize);
        }
        
        // Allocate scratch buffers for parallel processing
        scratchBuffer.setSize(2, maxBlockSize);
        parallelBuffer.setSize(2, maxBlockSize);
    }
    
    /** Processes audio through the routing graph */
    void process(AudioBuffer<float>& buffer)
    {
        if (nodes.empty())
            return;
        
        // Process starting from node 0
        processNode(0, buffer);
    }
    
    /** Resets all nodes */
    void reset()
    {
        for (auto& node : nodes)
        {
            if (node.module != nullptr)
                node.module->reset();
        }
    }
    
    //==============================================================================
    // Utility
    
    /** Returns number of nodes */
    int getNumNodes() const { return static_cast<int>(nodes.size()); }
    
    /** Gets a node's module */
    AudioModule* getModule(int nodeIndex)
    {
        if (nodeIndex >= 0 && nodeIndex < (int)nodes.size())
            return nodes[nodeIndex].module.get();
        return nullptr;
    }
    
    /** Clears the entire graph */
    void clear()
    {
        nodes.clear();
    }

private:
    std::vector<RoutingNode> nodes;
    AudioBuffer<float> scratchBuffer;
    AudioBuffer<float> parallelBuffer;
    std::vector<bool> processedNodes;
    
    void processNode(int nodeIndex, AudioBuffer<float>& buffer)
    {
        if (nodeIndex < 0 || nodeIndex >= (int)nodes.size())
            return;
        
        auto& node = nodes[nodeIndex];
        
        // Process this node's module
        if (node.module != nullptr && !node.module->isBypassed())
        {
            node.module->process(buffer);
        }
        
        // Handle routing based on mode
        if (node.connections.empty())
            return;  // End of chain
        
        switch (node.mode)
        {
            case RoutingMode::Serial:
                // Simple serial: just process next node
                if (!node.connections.empty())
                    processNode(node.connections[0], buffer);
                break;
                
            case RoutingMode::Parallel:
            {
                // Parallel: split signal, process each path, mix back
                int numConnections = (int)node.connections.size();
                if (numConnections == 0)
                    break;
                
                // Copy input for each parallel path
                parallelBuffer.makeCopyOf(buffer);
                buffer.clear();
                
                for (int i = 0; i < numConnections; ++i)
                {
                    AudioBuffer<float> pathBuffer;
                    pathBuffer.setDataToReferTo(
                        parallelBuffer.getArrayOfWritePointers(),
                        parallelBuffer.getNumChannels(),
                        parallelBuffer.getNumSamples()
                    );
                    
                    processNode(node.connections[i], pathBuffer);
                    
                    // Mix back with level control
                    float gain = 1.0f / std::sqrt((float)numConnections);  // Equal power
                    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                    {
                        buffer.addFrom(ch, 0, pathBuffer, ch, 0,
                                      pathBuffer.getNumSamples(), gain * node.mixLevel);
                    }
                }
                break;
            }
                
            case RoutingMode::SendReturn:
            {
                // Send/Return: dry signal continues, wet signal is mixed back
                scratchBuffer.makeCopyOf(buffer);  // Copy for send
                
                // Process main path (dry)
                if (!node.connections.empty())
                    processNode(node.connections[0], buffer);
                
                // Process send path (wet) if there's a second connection
                if (node.connections.size() > 1)
                {
                    processNode(node.connections[1], scratchBuffer);
                    
                    // Mix wet back into dry
                    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                    {
                        buffer.addFrom(ch, 0, scratchBuffer, ch, 0,
                                      scratchBuffer.getNumSamples(), node.mixLevel);
                    }
                }
                break;
            }
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoutingGraph)
};
