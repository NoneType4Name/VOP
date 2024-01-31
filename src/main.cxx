#ifdef _DEBUG
#    define DEBUG               true
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#    define DEBUG               false
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#endif

// #include <engine_setup.hxx>
#include <engine.hxx>
#include <ehi.hxx>
#include <spdlog/spdlog.h>
#include <iostream>

namespace
{
    struct _
    {
        _()
        {
            try
            {
                spdlog::set_level( DEBUG ? spdlog::level::trace : spdlog::level::critical );
                spdlog::set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
                SPDLOG_DEBUG( "--- Start logging ---" );
            }
            catch ( const std::exception &logInitE )
            {
                std::cerr << "Logger initialize error:\t" << logInitE.what() << "\n.";
                exit( EXIT_FAILURE );
            }
        }
        ~_()
        {
            SPDLOG_DEBUG( "--- Logging end ---" );
            spdlog::shutdown();
        }
    } _;
} // namespace

namespace Game
{
    class E;

    struct W : public Engine::window::window
    {
        void eventCallBack( int key, int scancode, int action, int mods ) override
        {
            switch ( key )
            {
                case GLFW_KEY_F11:
                    auto d { getDisplayResolution() };
                    if ( action == GLFW_RELEASE )
                    {
                        if ( properties.size.width != getDisplayResolution().width )
                        {
                            updateProperties( { .size = d, .fullScreenRefreshRate = 60 } );
                            break;
                        }
                        updateProperties( { .size = { .width = 800, .height = 600 }, .fullScreenRefreshRate = 0 } );
                    }
            }
        }

        void resizeCallBack( int width, int height ) override
        {
        }

      public:
        W( Engine::instance *instance, Engine::window::settings settings ) :
            Engine::window::window( 1, instance, settings )
        {
            data->createSurface( 0, 0 );
        }
    };

    struct E : public Engine::instance
    {
        E( const char *appName, uint32_t appVersion ) :
            // Engine::instance( 1, appName, appVersion )
            Engine::instance( appName, appVersion )
        {
            // VkApplicationInfo ApplicationInfo {};
            // ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            // ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
            // ApplicationInfo.pApplicationName   = appName;
            // ApplicationInfo.applicationVersion = appVersion;
            // VkInstanceCreateInfo InstanceCreateInfo {};
            // InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            // InstanceCreateInfo.pNext                   = nullptr;
            // InstanceCreateInfo.enabledLayerCount       = 0;
            // InstanceCreateInfo.ppEnabledLayerNames     = nullptr;
            // InstanceCreateInfo.enabledExtensionCount   = 0;
            // InstanceCreateInfo.ppEnabledExtensionNames = nullptr;
            // InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
            // data->create( InstanceCreateInfo );
        }
    };
    std::unique_ptr<E> engine { new E { "test", 0 } };

    // Engine::types::shader vertexShader { device->CreateShader( "./assets/shaders/binary.vert.spv", "main", Engine::ShaderStage::VERTEX_SHADER_TYPE ) };
    // Engine::types::shader fragmentShader { device->CreateShader( "./assets/shaders/binary.frag.spv", "main", Engine::ShaderStage::FRAGMENT_SHADER_TYPE ) };
    // Engine::types::pass pass { engine->CreateRenderPass( swapchain ) };
    // Engine::types::pipeline pipeline { device->CreatePipeline( layout, { vertexShader, fragmentShader }, pass ) };
} // namespace Game

int main()
{
    auto wnd { new Game::W { Game::engine.get(), { 800, 600, "test", 0, 1 } } };
    auto device { new Engine::device { Game::engine->getDevices()[ 0 ], { wnd } } };
    auto swapchain { device->getLink( wnd ) };
    VkAttachmentDescription ColorAttachment {};
    ColorAttachment.format         = swapchain->format.format;
    ColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription ColorAttachmentResolve {};
    ColorAttachmentResolve.format         = swapchain->format.format;
    ColorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription DepthAttachment {};
    DepthAttachment.format         = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription Attachments[] { ColorAttachment, DepthAttachment, ColorAttachmentResolve };

    VkAttachmentReference ColorAttachmentRef {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference DepthAttachmentRef {};
    DepthAttachmentRef.attachment = 1;
    DepthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference ColorAttachmentResolveRef {};
    ColorAttachmentResolveRef.attachment = 2;
    ColorAttachmentResolveRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &ColorAttachmentRef;
    subpass.pResolveAttachments     = &ColorAttachmentResolveRef;
    subpass.pDepthStencilAttachment = &DepthAttachmentRef;

    VkSubpassDependency dependency {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = sizeof( Attachments ) / sizeof( Attachments[ 0 ] );
    renderPassInfo.pAttachments    = Attachments;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;
    auto a { new Engine::renderPass { device, renderPassInfo } };

    while ( !wnd->shouldClose() )
    {
        wnd->updateEvents();
    }
    // Engine::types::device device { engine->CreateDevice( devices[ 0 ] ) };
    // Engine::types::Device
    // engine.InitDevice( devices[ 0 ] );
    // auto rectangle{ Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    // auto rectangle_texture { Engine::CreateTexture( "./assets/textures/rectangle/model.png" ) };
    // auto rectangle_model { Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    // auto devices { Engine::GetGraphicDevices( Engine::DISCRETE_GPU | Engine::INTEGRATED_GPU ) };
    // auto pipeline { Engine::CreatePipeline(
    //     { { Engine::CreateShader( "./assets/shaders/binary.vert.spv", "main", Engine::ShaderStage::VERTEX_SHADER_TYPE ),
    //         Engine::CreateShader( "./assets/shaders/binary.frag.spv", "main", Engine::ShaderStage::FRAGMENT_SHADER_TYPE ) } } ) };
    // Engine::AppCreateInfo App {};
    // App.width                       = 0;
    // App.height                      = 0;
    // App.title                       = "Quest App.";
    // App.device                      = devices.front();
    // App.settings.MultiSamplingCount = Engine::MultiSamplingCount::SAMPLE_COUNT_2_BIT;

    // Engine::init( App );
    // Engine::window::setKeyEventsCallback( []( int key, int scancode, int action, int mods )
    //                                       { SPDLOG_DEBUG( "key pressed code: {}", std::to_string( key ) ); } );
    // while ( !Engine::window::shouldClose() )
    // {
    //     Engine::window::updateEvents();
    // }
    // Engine::shutdown();
    return 0;
}
