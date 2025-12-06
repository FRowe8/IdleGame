/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019-2023 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#ifndef RMLUI_RENDERER_GL2_H
#define RMLUI_RENDERER_GL2_H

#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <SDL_opengl.h>
#endif

/**
 * RmlUi OpenGL 2.1 Renderer Backend
 * Implements the RenderInterface using legacy OpenGL
 */

class RenderInterface_GL2 : public Rml::RenderInterface {
public:
    RenderInterface_GL2();
    ~RenderInterface_GL2();

    // RenderInterface overrides
    void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices,
                        Rml::TextureHandle texture, const Rml::Vector2f& translation) override;

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices,
                                                 int* indices, int num_indices, Rml::TextureHandle texture) override;

    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(int x, int y, int width, int height) override;

    bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions,
                     const Rml::String& source) override;

    bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source,
                         const Rml::Vector2i& source_dimensions) override;

    void ReleaseTexture(Rml::TextureHandle texture_handle) override;

    void SetTransform(const Rml::Matrix4f* transform) override;

    // Additional methods
    void SetViewport(int width, int height);
    void BeginFrame();
    void EndFrame();

private:
    struct CompiledGeometry {
        GLuint vertex_buffer;
        GLuint index_buffer;
        int num_indices;
        Rml::TextureHandle texture;
    };

    int viewport_width_;
    int viewport_height_;
    Rml::Matrix4f transform_;
    bool transform_enabled_;
};

namespace RmlGL2 {

// Initialize OpenGL renderer
bool Initialize();

// Shutdown and cleanup
void Shutdown();

} // namespace RmlGL2

#endif // RMLUI_RENDERER_GL2_H
