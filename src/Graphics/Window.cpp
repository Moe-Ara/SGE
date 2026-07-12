#include "Window.h"
#include "../Exceptions/SGE_Exception.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string_view>

namespace SGE::GRAPHICS {

    namespace {
        bool environmentFlag(const char* name) {
            const char* value = std::getenv(name);
            return value && (std::string_view{value} == "1" ||
                             std::string_view{value} == "true" ||
                             std::string_view{value} == "TRUE");
        }

        std::string displayDiagnostics(const char* action) {
            const char* description = nullptr;
            const int error = glfwGetError(&description);
            const char* display = std::getenv("DISPLAY");
            const char* wayland = std::getenv("WAYLAND_DISPLAY");
            std::ostringstream message;
            message << action << " (GLFW error 0x" << std::hex << error << std::dec;
            if (description) message << ": " << description;
            message << "). DISPLAY=" << (display ? display : "<unset>")
                    << ", WAYLAND_DISPLAY=" << (wayland ? wayland : "<unset>")
                    << ". Set SGE_HEADLESS=1 only for an intentional no-window launch.";
            return message.str();
        }
    }

    Window& Window::getInstance(const std::string& title, int width, int height) {
        static Window instance(title, width, height);
        return instance;
    }

    Window::Window(const std::string& title, int width, int height)
            : m_title(title), m_width(width), m_height(height),
              m_framebufferWidth(width), m_framebufferHeight(height),
              m_window(nullptr), m_headless(false) {
        init();
    }

    Window::~Window() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

void Window::init() {
    if (environmentFlag("SGE_HEADLESS")) {
        m_headless = true;
        std::cerr << "[SGE] Explicit headless mode enabled by SGE_HEADLESS.\n";
        return;
    }

#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11) && defined(GLFW_PLATFORM_WAYLAND)
    if (const char* platform = std::getenv("SGE_GLFW_PLATFORM")) {
        if (std::string_view{platform} == "x11") {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        } else if (std::string_view{platform} == "wayland") {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        }
    } else if (std::getenv("WSL_INTEROP") && std::getenv("DISPLAY")) {
        // WSLg exposes both backends. GLFW Wayland can successfully create an EGL
        // context yet fail to map a visible top-level surface in remote IDE sessions;
        // its XWayland endpoint is the more reliable default there.
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    }
#endif

    if (!glfwInit()) {
        const std::string diagnostic = displayDiagnostics("Failed to initialize GLFW");
        glfwTerminate();
        throw SGE::EXCEPTIONS::SGE_Exception(diagnostic);
    }

    // Request an OpenGL 3.3 Core Profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(
        m_width,
        m_height,
        m_title.c_str(),
        nullptr,
        nullptr
    );

    if (!m_window) {
        const std::string diagnostic = displayDiagnostics("Failed to create GLFW window");
        glfwTerminate();
        throw SGE::EXCEPTIONS::SGE_Exception(diagnostic);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, windowResize);
    glfwSetFramebufferSizeCallback(m_window, framebufferResize);

    glfwGetFramebufferSize(m_window, &m_framebufferWidth, &m_framebufferHeight);

    glfwShowWindow(m_window);
    glfwFocusWindow(m_window);
    glfwRequestWindowAttention(m_window);

    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        throw SGE::EXCEPTIONS::SGE_Exception("Failed to initialize GLAD");
    }

#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11) && defined(GLFW_PLATFORM_WAYLAND)
    const int selectedPlatform = glfwGetPlatform();
    const char* platformName = selectedPlatform == GLFW_PLATFORM_X11
        ? "X11"
        : (selectedPlatform == GLFW_PLATFORM_WAYLAND ? "Wayland" : "Other");
    std::cerr << "[SGE] GLFW platform: " << platformName << '\n';
#endif

    // Commit a frame before expensive shader/environment setup. Wayland compositors
    // may not map a newly created window until its first buffer swap and event poll.
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    glClearColor(0.05f, 0.07f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_window);
    glfwPollEvents();

    std::cerr << "[SGE] OpenGL vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cerr << "[SGE] OpenGL renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cerr << "[SGE] OpenGL version: " << glGetString(GL_VERSION) << '\n';
}

    void Window::update() {
        if (m_headless || !m_window) {
            return;
        }
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Window::pollEvents() const {
        if (!m_headless && m_window) {
            glfwPollEvents();
        }
    }

    void Window::present() const {
        if (!m_headless && m_window) {
            glfwSwapBuffers(m_window);
        }
    }

    void Window::clear() const {
        if (m_headless || !m_window) {
            return;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::closed() const {
        if (m_headless || !m_window) {
            return true;
        }
        return glfwWindowShouldClose(m_window);
    }

    bool Window::isHeadless() const {
        return m_headless;
    }

    GLFWwindow* Window::getMWindow() const {
        return m_window;
    }

    int Window::getHeight() const {
        return m_height;
    }

    int Window::getWidth() const {
        return m_width;
    }

    int Window::getFramebufferHeight() const {
        return m_framebufferHeight;
    }

    int Window::getFramebufferWidth() const {
        return m_framebufferWidth;
    }

    void Window::windowResize(GLFWwindow* window, int width, int height) {
        if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
            self->m_width = width;
            self->m_height = height;
        }
    }

    void Window::framebufferResize(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        if (auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
            self->m_framebufferWidth = width;
            self->m_framebufferHeight = height;
        }
    }
}
