//Compile with: tcc -lopengl32 -lglu32 -lglut32 -Wl,-subsystem=windows lorenz.c

/**************************
 * Includes
 *
 **************************/
#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>

// Constants for Lorenz attractor
const float sigma = 10.0;
const float rho = 28.0;
const float beta = 8.0 / 3.0;
const float dt = 0.01;
float x = 0.1, y = 0.0, z = 0.0;

const int width = 800;
const int height = 600;

float f = 0.4;
int showaxes = -1;

BOOL FullScreen = FALSE;
WINDOWPLACEMENT wpc;

void drawAxes() {
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.6, 0.0, 0.0); glVertex3f(-3.0, 0.0, 0.0); glVertex3f(3.0, 0.0, 0.0);
    glColor3f(0.0, 0.6, 0.0); glVertex3f(0.0, -3.0, 0.0); glVertex3f(0.0, 3.0, 0.0);
    glColor3f(0.0, 0.0, 0.6); glVertex3f(0.0, 0.0, -3.0); glVertex3f(0.0, 0.0, 3.0);
    glEnd();
}

void drawLorenzAttractor() {
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 10000; i++) {
        float dx = sigma * (y - x) * dt;
        float dy = (x * (rho - z) - y) * dt;
        float dz = (x * y - beta * z) * dt;
        x += dx;
        y += dy;
        z += dz;

        // Calculate the distance from the origin to determine color intensity
        float distance = sqrt(x * x + y * y + z * z);

        // Normalize the coordinates for color mapping
        float r = fabs(sin(distance * 0.1f));  // Red based on distance
        float g = fabs(cos(distance * 0.1f));  // Green based on distance
        float b = fabs(sin(distance * 0.1f + 2.0f));  // Blue based on distance with phase shift

        glColor3f(r, g, b);

        glVertex3f(x * 0.05, y * 0.05, 1-z * 0.05);
    }
    glEnd();
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Init() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    changeSize(width, height);
    // Center the Lorenz attractor by translating it to the origin
    // These translation values are approximations based on the known bounds of the Lorenz attractor
    glTranslatef(0, -0.1, -6.0); // Adjust the Z value to zoom and the X, Y values to center

    // Rotate the attractor for better viewing
    glRotatef(90.0, 1.0, 1.0, 1.0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "LorenzAttractor";
    RegisterClass(&wc);

    hWnd = CreateWindow("LorenzAttractor", "Lorenz Attractor",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0, 0, width, height,
        NULL, NULL, hInstance, NULL);

    EnableOpenGL(hWnd, &hDC, &hRC);
    Init();
    float a = 0;
    int b = 1;

    while (!bQuit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (showaxes == 1) drawAxes();
            drawLorenzAttractor();

            glRotatef(f, sin(a), 0, 0);
            glRotatef(f, 0, cos(a), 0);
            glRotatef(f, 0, 0, 1 - sin(a));

            a += b * 0.001;
            if (a > 45) b = -1;
            if (a < -45) b = 1;

            //glRotatef(f,0,1,0);

            SwapBuffers(hDC);
            Sleep(4);
        }
    }

    DisableOpenGL(hWnd, hDC, hRC);
    DestroyWindow(hWnd);
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE: {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        changeSize(width, height);
    }
                return 0;
    case WM_DESTROY:
        return 0;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        case VK_UP:
            f += 0.1;
            return 0;
        case VK_DOWN:
            f -= 0.1;
            return 0;
        case 'A':
            showaxes = -showaxes;
            return 0;
        case VK_SPACE: {
            if (!FullScreen) {
                GetWindowPlacement(hWnd, &wpc);
                SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
                SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
                FullScreen = TRUE;
            }
            else {
                SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
                SetWindowLong(hWnd, GWL_EXSTYLE, 0L);
                SetWindowPlacement(hWnd, &wpc);
                ShowWindow(hWnd, SW_SHOWDEFAULT);
                FullScreen = FALSE;
            }
        }
                         return 0;
        }
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    *hDC = GetDC(hWnd);

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
