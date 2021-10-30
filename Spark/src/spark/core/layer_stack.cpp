#include "layer_stack.h"

#include <algorithm>

namespace Spark
{
void LayerStack::PushLayer(Layer *layer)
{
    m_pendingLayers.insert(m_pendingLayers.begin() + m_lastLayerIndex, layer);
    m_lastLayerIndex++;
}

void LayerStack::PushLayerBefore(Layer *layer, Layer *beforeLayer)
{
    auto found_it = std::find(m_pendingLayers.begin(), m_pendingLayers.begin() + m_lastLayerIndex, beforeLayer);
    if (found_it != m_pendingLayers.begin() + m_lastLayerIndex)
    {
        m_pendingLayers.insert(found_it, layer);
        m_lastLayerIndex++;
    }
}

void LayerStack::PushLayerAfter(Layer *layer, Layer *afterLayer)
{
    auto found_it = std::find(m_pendingLayers.begin(), m_pendingLayers.begin() + m_lastLayerIndex, afterLayer);
    if (found_it != m_pendingLayers.begin() + m_lastLayerIndex)
    {
        m_pendingLayers.insert(found_it + 1, layer);
        m_lastLayerIndex++;
    }
}

void LayerStack::PushFirstLayer(Layer *layer)
{
    m_pendingLayers.insert(m_pendingLayers.begin(), layer);
    m_lastLayerIndex++;
}

void LayerStack::PushOverlayLayer(Layer *layer)
{
    m_pendingLayers.push_back(layer);
}

void LayerStack::PopLayer(Layer *layer)
{
    auto found_it = std::find(m_pendingLayers.begin(), m_pendingLayers.begin() + m_lastLayerIndex, layer);
    if (found_it != m_pendingLayers.begin() + m_lastLayerIndex)
    {
        m_pendingLayers.erase(found_it);
        m_lastLayerIndex--;
    }
}

void LayerStack::PopOverlayLayer(Layer *layer)
{
    auto found_it = std::find(m_pendingLayers.begin() + m_lastLayerIndex, m_pendingLayers.end(), layer);
    if (found_it != m_pendingLayers.end())
    {
        m_pendingLayers.erase(found_it);
    }
}

void LayerStack::updatePendings()
{
    m_layers = m_pendingLayers;
}

} // namespace Spark
