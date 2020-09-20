#include "layer_stack.h"

#include <algorithm>

namespace Spark
{
void LayerStack::PushLayer(Layer *layer)
{
    m_layers.insert(m_layers.begin() + m_lastLayerIndex, layer);
    m_lastLayerIndex++;
}

void LayerStack::PushLayerBefore(Layer *layer, Layer *beforeLayer)
{
    auto found_it = std::find(m_layers.begin(), m_layers.begin() + m_lastLayerIndex, beforeLayer);
    if (found_it != m_layers.begin() + m_lastLayerIndex)
    {
        m_layers.insert(found_it, layer);
        m_lastLayerIndex++;
    }
}

void LayerStack::PushLayerAfter(Layer *layer, Layer *afterLayer)
{
    auto found_it = std::find(m_layers.begin(), m_layers.begin() + m_lastLayerIndex, afterLayer);
    if (found_it != m_layers.begin() + m_lastLayerIndex)
    {
        m_layers.insert(found_it + 1, layer);
        m_lastLayerIndex++;
    }
}

void LayerStack::PushFirstLayer(Layer *layer)
{
    m_layers.insert(m_layers.begin(), layer);
    m_lastLayerIndex++;
}

void LayerStack::PushOverlayLayer(Layer *layer)
{
    m_layers.push_back(layer);
}

void LayerStack::PopLayer(Layer *layer)
{
    auto found_it = std::find(m_layers.begin(), m_layers.begin() + m_lastLayerIndex, layer);
    if (found_it != m_layers.begin() + m_lastLayerIndex)
    {
        m_layers.erase(found_it);
        m_lastLayerIndex--;
    }
}

void LayerStack::PopOverlayLayer(Layer *layer)
{
    auto found_it = std::find(m_layers.begin() + m_lastLayerIndex, m_layers.end(), layer);
    if (found_it != m_layers.end())
    {
        m_layers.erase(found_it);
    }
}

} // namespace Spark
