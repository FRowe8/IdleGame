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

#include "RmlUi_Renderer_GL2.h"
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#include <stb_image.h>
#pragma GCC diagnostic pop

// ============================================================================
// RenderInterface_GL2 Implementation
// ============================================================================

static RenderInterface_GL2* g_render_interface = nullptr;

RenderInterface_GL2::RenderInterface_GL2()
    : viewport_width_(0)
    , viewport_height_(0)
    , transform_enabled_(false)
{
}

RenderInterface_GL2::~RenderInterface_GL2() {
}

void RenderInterface_GL2::SetViewport(int width, int height) {
    viewport_width_ = width;
    viewport_height_ = height;
}

void RenderInterface_GL2::BeginFrame() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport_width_, viewport_height_, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void RenderInterface_GL2::EndFrame() {
}

void RenderInterface_GL2::RenderGeometry(Rml::Vertex* vertices, int num_vertices,
                                         int* indices, int num_indices,
                                         Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);

    if (transform_enabled_) {
        glMultMatrixf(transform_.data());
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if (texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), &vertices[0].position);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), &vertices[0].colour);

    if (texture) {
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), &vertices[0].tex_coord);
    }

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (texture) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glPopMatrix();
}

Rml::CompiledGeometryHandle RenderInterface_GL2::CompileGeometry(Rml::Vertex* vertices, int num_vertices,
                                                                   int* indices, int num_indices,
                                                                   Rml::TextureHandle texture)
{
    CompiledGeometry* geometry = new CompiledGeometry();
    geometry->num_indices = num_indices;
    geometry->texture = texture;

    glGenBuffers(1, &geometry->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Rml::Vertex), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &geometry->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(int), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return (Rml::CompiledGeometryHandle)geometry;
}

void RenderInterface_GL2::RenderCompiledGeometry(Rml::CompiledGeometryHandle handle,
                                                   const Rml::Vector2f& translation)
{
    CompiledGeometry* geometry = (CompiledGeometry*)handle;

    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);

    if (transform_enabled_) {
        glMultMatrixf(transform_.data());
    }

    if (geometry->texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, (GLuint)geometry->texture);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->index_buffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, position));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, colour));

    if (geometry->texture) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, tex_coord));
    }

    glDrawElements(GL_TRIANGLES, geometry->num_indices, GL_UNSIGNED_INT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (geometry->texture) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPopMatrix();
}

void RenderInterface_GL2::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle handle) {
    CompiledGeometry* geometry = (CompiledGeometry*)handle;
    glDeleteBuffers(1, &geometry->vertex_buffer);
    glDeleteBuffers(1, &geometry->index_buffer);
    delete geometry;
}

void RenderInterface_GL2::EnableScissorRegion(bool enable) {
    if (enable) {
        glEnable(GL_SCISSOR_TEST);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void RenderInterface_GL2::SetScissorRegion(int x, int y, int width, int height) {
    glScissor(x, viewport_height_ - (y + height), width, height);
}

bool RenderInterface_GL2::LoadTexture(Rml::TextureHandle& texture_handle,
                                       Rml::Vector2i& texture_dimensions,
                                       const Rml::String& source)
{
    int width, height, channels;
    unsigned char* data = stbi_load(source.c_str(), &width, &height, &channels, 4);

    if (!data) {
        return false;
    }

    texture_dimensions.x = width;
    texture_dimensions.y = height;

    bool success = GenerateTexture(texture_handle, data, texture_dimensions);

    stbi_image_free(data);
    return success;
}

bool RenderInterface_GL2::GenerateTexture(Rml::TextureHandle& texture_handle,
                                           const Rml::byte* source,
                                           const Rml::Vector2i& source_dimensions)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, source_dimensions.x, source_dimensions.y,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, source);

    texture_handle = (Rml::TextureHandle)texture_id;
    return true;
}

void RenderInterface_GL2::ReleaseTexture(Rml::TextureHandle texture_handle) {
    GLuint texture_id = (GLuint)texture_handle;
    glDeleteTextures(1, &texture_id);
}

void RenderInterface_GL2::SetTransform(const Rml::Matrix4f* transform) {
    if (transform) {
        transform_ = *transform;
        transform_enabled_ = true;
    } else {
        transform_enabled_ = false;
    }
}

// ============================================================================
// RmlGL2 Namespace
// ============================================================================

// External global for main.cpp
RenderInterface_GL2* g_render_interface_gl2 = nullptr;

bool RmlGL2::Initialize() {
    if (!g_render_interface) {
        g_render_interface = new RenderInterface_GL2();
        g_render_interface_gl2 = g_render_interface;
        Rml::SetRenderInterface(g_render_interface);
    }
    return true;
}

void RmlGL2::Shutdown() {
    if (g_render_interface) {
        Rml::SetRenderInterface(nullptr);
        delete g_render_interface;
        g_render_interface = nullptr;
        g_render_interface_gl2 = nullptr;
    }
}
