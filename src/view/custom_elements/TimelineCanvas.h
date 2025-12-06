#pragma once

#include <RmlUi/Core/Element.h>

namespace paradox::view {

/**
 * @brief Custom RmlUi element for visual timeline rendering
 *
 * PRODUCTION CONSTRAINT: Visual Interaction
 * ==========================================
 * The "Paradox Protocol" UI cannot just be buttons.
 * We need a visual representation of time loops.
 *
 * This custom element will draw:
 * - Timeline axis (past → present → future)
 * - Loop arcs showing resource transfers
 * - Paradox instability indicators
 *
 * Future Implementation:
 * - Extend Rml::Element
 * - Override OnRender() to draw custom graphics
 * - Use SDL2 renderer or OpenGL for visualization
 */
class TimelineCanvas {
public:
    // TODO: Implement custom RmlUi element
    // For now, this is a stub placeholder
};

} // namespace paradox::view
