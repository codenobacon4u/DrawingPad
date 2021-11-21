#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DrawingPad.h>
#include <fstream>
#include <chrono>
#include <iostream>
#include <glm/glm/gtx/string_cast.hpp>
#include <glm/glm/gtx/rotate_vector.hpp>
#include <imgui/imgui.h>

#include "ImGuiWindow.h"

API Curr_API = API::Vulkan;

static uint32_t __glsl_shader_vert_spv[] =
{
    0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

static uint32_t __glsl_shader_frag_spv[] =
{
    0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};

struct Vertex {
    glm::vec2 pos;
    glm::vec2 tex;
    glm::vec4 color;
};

bool rebuildSwap = false;

bool showDemo = true;
bool showAnother = false;
ImVec4 clear = ImVec4(0.45f, 0.55f, 0.6f, 1.0f);

Texture* fontImage;
TextureView* fontView;

int main() {
	GraphicsDevice* gd;
	GraphicsContext* ctx;
	Swapchain* swap;
	Buffer* vb = nullptr;
	Buffer* ib = nullptr;
	Buffer* ub = nullptr;
	Pipeline* pipeline;
	Shader* vertShader;
	Shader* fragShader;
	if (!glfwInit())
		printf("Failed");
	if (Curr_API != API::OpenGL)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "DrawingPad Test", NULL, NULL);
	if (window == NULL) {
		printf("Window is NULL!\n");
		std::abort();
	}
    // Setup Graphics
	gd = GraphicsDevice::Create(window, Curr_API);
	GraphicsContextDesc desc = {};
	desc.ContextID = 0;
	desc.Name = "Immediate";
	ctx = gd->CreateContext(desc);
    // Create Swapchain
	SwapchainDesc swapSpec;
	swap = gd->CreateSwapchain(swapSpec, ctx, window);

    // ======== Create Shaders ========
    ShaderDesc sDesc = {};
    sDesc.EntryPoint = "main";
    sDesc.Name = "Basic Vert";
    //sDesc.Src = vertSrc;
    sDesc.Path = "shaders/ui.vert";
    sDesc.Type = ShaderType::Vertex;
    vertShader = gd->CreateShader(sDesc);
    sDesc.Name = "Basic Frag";
    //sDesc.Src = fragSrc;
    sDesc.Path = "shaders/ui.frag";
    sDesc.Type = ShaderType::Fragment;
    fragShader = gd->CreateShader(sDesc);

    LayoutElement vertInputs[]{
        {
            0, // InputIndex Location
            0, // BufferSlot Binding
            2, // Num Components
            offsetof(Vertex, pos), // Offset
            sizeof(Vertex) // Stride
        },
        {
            1, // InputIndex Location
            0, // BufferSlot Binding
            2, // Num Components
            offsetof(Vertex, tex),  // Offset
            sizeof(Vertex) // Stride
        },
        {
            2,
            0,
            4,
            offsetof(Vertex, color),
            sizeof(Vertex)
        }
    };

    GraphicsPipelineDesc pDesc = {};
    pDesc.NumViewports = 1;
    pDesc.NumColors = 1;
    pDesc.ColorFormats[0] = swap->GetDesc().ColorFormat;
    pDesc.DepthFormat = swap->GetDesc().DepthFormat;
    pDesc.InputLayout.NumElements = 3;
    pDesc.InputLayout.Elements = vertInputs;
    pDesc.ShaderCount = 2;
    pDesc.Shaders[0] = vertShader;
    pDesc.Shaders[1] = fragShader;
    pipeline = gd->CreateGraphicsPipeline(pDesc);

    // Setup Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backents
    //ImGui_ImplGlfw_InitForVulkan(window, true);
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    // Upload Fonts
    {
        // Get Command Buffer
        // Create Font Texture
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        size_t upload_size = (size_t)width * (size_t)height * 4 * sizeof(char);
        {
            TextureDesc desc = {};
            desc.Type = TextureType::DimTex2D;
            desc.Format = TextureFormat::RGBA8Unorm;
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleCount = 1;
            desc.BindFlags = BindFlags::ShaderResource;
            fontImage = gd->CreateTexture(desc, pixels);
        }

        {
            TextureViewDesc desc = {};
            desc.Format = TextureFormat::RGBA8Unorm;
            fontView = fontImage->CreateView(desc);
        }

        io.Fonts->TexID = (ImTextureID)fontImage;
    }
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (rebuildSwap)
        {
            //resize
            rebuildSwap = false;
        }

        // API NewFrame()
        {/*Noting to do*/}
        // GLFW NewFrame()
        ImGui_ImplGlfw_NewFrame();

        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &showDemo);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &showAnother);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if (showAnother)
        {
            ImGui::Begin("Another Window", &showAnother);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                showAnother = false;
            ImGui::End();
        }

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        const bool mini = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

        if (!mini && drawData->TotalVtxCount > 0)
        {
            uint32_t i = swap->GetNextBackbuffer().first;
            TextureView* rtv = swap->GetNextBackbuffer().second;
            TextureView* dsv = swap->GetDepthBufferView();
            float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
            ctx->Begin(i);
            ctx->SetRenderTargets(1, &rtv, dsv);
            ctx->ClearColor(rtv, nullptr);
            uint32_t vtxSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
            uint32_t idxSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
            if (!vb || vb->GetSize() < vtxSize)
            {
                if (!vb)
                    delete vb;
                BufferDesc buffDesc = {};
                buffDesc.Usage = BufferUsageFlags::Default;
                buffDesc.BindFlags = BufferBindFlags::Vertex;
                buffDesc.Size = vtxSize;
                ImDrawVert* dst = new ImDrawVert[drawData->TotalVtxCount];
                for (int32_t i = 0; i < drawData->CmdListsCount; i++)
                {
                    const ImDrawList* cmdLst = drawData->CmdLists[i];
                    memcpy(dst, cmdLst->VtxBuffer.Data, cmdLst->VtxBuffer.Size * sizeof(ImDrawVert));
                    dst += cmdLst->VtxBuffer.Size;
                }
                vb = gd->CreateBuffer(buffDesc, dst);
                delete[] dst;
            }
            if (!ib || ib->GetSize() < idxSize)
            {
                if (!ib)
                    delete ib;
                BufferDesc buffDesc = {};
                buffDesc.Usage = BufferUsageFlags::Default;
                buffDesc.BindFlags = BufferBindFlags::Index;
                buffDesc.Size = idxSize;
                ImDrawIdx* dst = new ImDrawIdx[drawData->TotalIdxCount];
                for (int32_t i = 0; i < drawData->CmdListsCount; i++)
                {
                    const ImDrawList* cmdLst = drawData->CmdLists[i];
                    memcpy(dst, cmdLst->IdxBuffer.Data, cmdLst->IdxBuffer.Size * sizeof(ImDrawIdx));
                    dst += cmdLst->IdxBuffer.Size;
                }
                ib = gd->CreateBuffer(buffDesc, dst);
                delete[] dst;
            }
            // Get clear color
            // Render Frame
            ctx->SetPipeline(pipeline);
            Buffer* vertexBuffs[] = { vb };
            uint32_t offsets[] = { 0 };
            ctx->SetVertexBuffers(0, 1, vertexBuffs, offsets);
            ctx->SetIndexBuffer(ib, 0);

            ImVec2 clipOffset = drawData->DisplayPos;
            ImVec2 clipScale = drawData->FramebufferScale;
            uint32_t fbWidth = (uint32_t)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
            uint32_t fbHeight = (uint32_t)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
            uint32_t vtxOffset = 0, idxOffset = 0;
            for (int32_t i = 0; i < drawData->CmdListsCount; i++)
            {
                const ImDrawList* cmdList = drawData->CmdLists[i];
                for (int32_t j = 0; j < cmdList->CmdBuffer.Size; j++)
                {
                    const ImDrawCmd* cmd = &cmdList->CmdBuffer[j];
                    ImVec4 clipRect;
                    clipRect.x = (cmd->ClipRect.x - clipOffset.x) * clipScale.x;
                    clipRect.y = (cmd->ClipRect.y - clipOffset.y) * clipScale.y;
                    clipRect.x = (cmd->ClipRect.z - clipOffset.x) * clipScale.x;
                    clipRect.x = (cmd->ClipRect.w - clipOffset.y) * clipScale.y;

                    if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
                    {
                        if (clipRect.x < 0.0f)
                            clipRect.x = 0.0f;
                        if (clipRect.y < 0.0f)
                            clipRect.y = 0.0f;

                        ctx->SetScissors(1, (int32_t)clipRect.x, (int32_t)clipRect.y, (uint32_t)(clipRect.z - clipRect.x), (uint32_t)(clipRect.w - clipRect.y));
                        DrawAttribs attribs = {};
                        attribs.VrtIdxCount = cmd->ElemCount;
                        attribs.InstanceCount = 1;
                        attribs.FirstVrtIdx = cmd->IdxOffset + idxOffset;
                        attribs.VertexOffset = cmd->VtxOffset + vtxOffset;
                        attribs.FirstInstance = 0;
                        ctx->DrawIndexed(attribs);
                    }
                }
                vtxOffset += cmdList->VtxBuffer.Size;
                idxOffset += cmdList->IdxBuffer.Size;
            }
            // Present Frame
            swap->Present(0);
        }
    }

    ImGui::DestroyContext();
    //Cleanup API

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}