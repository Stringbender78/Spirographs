#pragma region license_and_help
#pragma region hello_world_example
// O------------------------------------------------------------------------------O
// | Example "Hello World" Program (main.cpp)                                     |
// O------------------------------------------------------------------------------O
/*

#define ALO_GE_APPLICATION
#include "aloGameEngine.h"

// Override base class with your custom functionality
class Example : public alo::GameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random colored pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}

*/
#pragma endregion

#ifndef ALO_PGE_DEF
#define ALO_PGE_DEF

#pragma region std_includes
// O------------------------------------------------------------------------------O
// | STANDARD INCLUDES                                                            |
// O------------------------------------------------------------------------------O
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <array>
#include <cstring>
#pragma endregion

#define GE_VER 220

// O------------------------------------------------------------------------------O
// | COMPILER CONFIGURATION ODDITIES                                              |
// O------------------------------------------------------------------------------O
#pragma region compiler_config
#define USE_EXPERIMENTAL_FS
#if defined(_WIN32)
	#if _MSC_VER >= 1920 && _MSVC_LANG >= 201703L
		#undef USE_EXPERIMENTAL_FS
	#endif
#endif
#if defined(__linux__) || defined(__MINGW32__) || defined(__EMSCRIPTEN__) || defined(__FreeBSD__) || defined(__APPLE__)
	#if __cplusplus >= 201703L
		#undef USE_EXPERIMENTAL_FS
	#endif
#endif

#if !defined(ALO_KEYBOARD_UK)
	#define ALO_KEYBOARD_UK
#endif


#if defined(USE_EXPERIMENTAL_FS) || defined(FORCE_EXPERIMENTAL_FS)
	// C++14
	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#include <experimental/filesystem>
	namespace _gfs = std::experimental::filesystem::v1;
#else
	// C++17
	#include <filesystem>
	namespace _gfs = std::filesystem;
#endif

#if defined(UNICODE) || defined(_UNICODE)
	#define aloT(s) L##s
#else
	#define aloT(s) s
#endif

#define UNUSED(x) (void)(x)

// O------------------------------------------------------------------------------O
// | PLATFORM SELECTION CODE:                                                     |
// O------------------------------------------------------------------------------O

// Platform
#if !defined(ALO_PLATFORM_WINAPI) && !defined(ALO_PLATFORM_X11) && !defined(ALO_PLATFORM_GLUT) && !defined(ALO_PLATFORM_EMSCRIPTEN)
	#if !defined(ALO_PLATFORM_CUSTOM_EX)
		#if defined(_WIN32)
			#define ALO_PLATFORM_WINAPI
		#endif
		#if defined(__linux__) || defined(__FreeBSD__)
			#define ALO_PLATFORM_X11
		#endif
		#if defined(__APPLE__)
			#define GL_SILENCE_DEPRECATION
			#define ALO_PLATFORM_GLUT
		#endif
		#if defined(__EMSCRIPTEN__)
			#define ALO_PLATFORM_EMSCRIPTEN
		#endif
	#endif
#endif

// Start Situation
#if defined(ALO_PLATFORM_GLUT) || defined(ALO_PLATFORM_EMSCRIPTEN)
	#define GE_USE_CUSTOM_START
#endif

// Renderer
#if !defined(ALO_GFX_OPENGL10) && !defined(ALO_GFX_OPENGL33) && !defined(ALO_GFX_DIRECTX10)
	#if !defined(ALO_GFX_CUSTOM_EX)
		#if defined(ALO_PLATFORM_EMSCRIPTEN)
			#define ALO_GFX_OPENGL33
		#else
			#define ALO_GFX_OPENGL10
		#endif
	#endif
#endif

// Image loader
#if !defined(ALO_IMAGE_STB) && !defined(ALO_IMAGE_GDI) && !defined(ALO_IMAGE_LIBPNG)
	#if !defined(ALO_IMAGE_CUSTOM_EX)
		#if defined(_WIN32)
			#define	ALO_IMAGE_GDI
		#endif
		#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__EMSCRIPTEN__)
			#define	ALO_IMAGE_LIBPNG
		#endif
	#endif
#endif


// O------------------------------------------------------------------------------O
// | PLATFORM-SPECIFIC DEPENDENCIES                                               |
// O------------------------------------------------------------------------------O
#if !defined(ALO_GE_HEADLESS)
#if defined(ALO_PLATFORM_WINAPI)	
	#define _WINSOCKAPI_ 
		#if !defined(VC_EXTRALEAN)
		#define VC_EXTRALEAN
	#endif
	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif

	// In Code::Blocks
	#if !defined(_WIN32_WINNT)
		#ifdef HAVE_MSMF
			#define _WIN32_WINNT 0x0600 // Windows Vista
		#else
			#define _WIN32_WINNT 0x0500 // Windows 2000
		#endif
	#endif

	#include <windows.h>
	#undef _WINSOCKAPI_
#endif

#if defined(ALO_PLATFORM_X11)
	namespace X11
	{
		#include <X11/X.h>
		#include <X11/Xlib.h>
	}
#endif

#if defined(ALO_PLATFORM_GLUT)
	#if defined(__linux__)
		#include <GL/glut.h>
		#include <GL/freeglut_ext.h>
	#endif
	#if defined(__APPLE__)
		#include <GLUT/glut.h>
	#include <objc/message.h>
	#include <objc/NSObjCRuntime.h>
	#endif
#endif
#endif
#pragma endregion

// O------------------------------------------------------------------------------O
// | aloGameEngine INTERFACE DECLARATION                                          |
// O------------------------------------------------------------------------------O
#pragma region ge_declaration
namespace alo
{
	class GameEngine;
	class Sprite;

	// Game Engine Advanced Configuration
	constexpr uint8_t  nMouseButtons = 5;
	constexpr uint8_t  nDefaultAlpha = 0xFF;
	constexpr uint32_t nDefaultPixel = (nDefaultAlpha << 24);
	constexpr uint8_t  nTabSizeInSpaces = 4;
	enum rcode { FAIL = 0, OK = 1, NO_FILE = -1 };

	// O------------------------------------------------------------------------------O
	// | alo::Pixel - Represents a 32-Bit RGBA color                                 |
	// O------------------------------------------------------------------------------O
	struct Pixel
	{
		union
		{
			uint32_t n = nDefaultPixel;
			struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
		};

		enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

		Pixel();
		Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = nDefaultAlpha);
		Pixel(uint32_t p);
		Pixel& operator = (const Pixel& v) = default;
		bool   operator ==(const Pixel& p) const;
		bool   operator !=(const Pixel& p) const;
		Pixel  operator * (const float i) const;
		Pixel  operator / (const float i) const;
		Pixel& operator *=(const float i);
		Pixel& operator /=(const float i);
		Pixel  operator + (const Pixel& p) const;
		Pixel  operator - (const Pixel& p) const;
		Pixel& operator +=(const Pixel& p);
		Pixel& operator -=(const Pixel& p);
		Pixel  inv() const;
	};

	Pixel PixelF(float red, float green, float blue, float alpha = 1.0f);
	Pixel PixelLerp(const alo::Pixel& p1, const alo::Pixel& p2, float t);


	// O------------------------------------------------------------------------------O
	// | USEFUL CONSTANTS                                                             |
	// O------------------------------------------------------------------------------O
	static const Pixel
		GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
		RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
		YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
		GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
		CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
		BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
		MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
		WHITE(255, 255, 255), BLACK(0, 0, 0), BLANK(0, 0, 0, 0);

	enum Key
	{
		NONE,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		UP, DOWN, LEFT, RIGHT,
		SPACE, TAB, SHIFT, CTRL, INS, DEL, HOME, END, PGUP, PGDN,
		BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
		NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
		NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
		EQUALS, COMMA, MINUS,
		OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,
		CAPS_LOCK, ENUM_END
	};

	namespace Mouse
	{
		static constexpr int32_t LEFT = 0;
		static constexpr int32_t RIGHT = 1;
		static constexpr int32_t MIDDLE = 2;
	};

	// O------------------------------------------------------------------------------O
	// | alo::HWButton - Represents the state of a hardware button (mouse/key/joy)    |
	// O------------------------------------------------------------------------------O
	struct HWButton
	{
		bool bPressed = false;	// Set once during the frame the event occurs
		bool bReleased = false;	// Set once during the frame the event occurs
		bool bHeld = false;		// Set true for all frames between pressed and released events
	};




	// O------------------------------------------------------------------------------O
	// | alo::vX2d - A generic 2D vector type                                         |
	// O------------------------------------------------------------------------------O
#if !defined(ALO_IGNORE_VEC2D)
	template <class T>
	struct v2d_generic
	{
		T x = 0;
		T y = 0;
		v2d_generic() : x(0), y(0) {}
		v2d_generic(T _x, T _y) : x(_x), y(_y) {}
		v2d_generic(const v2d_generic& v) : x(v.x), y(v.y) {}
		v2d_generic& operator=(const v2d_generic& v) = default;
		T mag() const { return T(std::sqrt(x * x + y * y)); }
		T mag2() const { return x * x + y * y; }
		v2d_generic  norm() const { T r = 1 / mag(); return v2d_generic(x * r, y * r); }
		v2d_generic  perp() const { return v2d_generic(-y, x); }
		v2d_generic  floor() const { return v2d_generic(std::floor(x), std::floor(y)); }
		v2d_generic  ceil() const { return v2d_generic(std::ceil(x), std::ceil(y)); }
		v2d_generic  max(const v2d_generic& v) const { return v2d_generic(std::max(x, v.x), std::max(y, v.y)); }
		v2d_generic  min(const v2d_generic& v) const { return v2d_generic(std::min(x, v.x), std::min(y, v.y)); }
		v2d_generic  cart() { return { std::cos(y) * x, std::sin(y) * x }; }
		v2d_generic  polar() { return { mag(), std::atan2(y, x) }; }
		T dot(const v2d_generic& rhs) const { return this->x * rhs.x + this->y * rhs.y; }
		T cross(const v2d_generic& rhs) const { return this->x * rhs.y - this->y * rhs.x; }
		v2d_generic  operator +  (const v2d_generic& rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
		v2d_generic  operator -  (const v2d_generic& rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
		v2d_generic  operator *  (const T& rhs)           const { return v2d_generic(this->x * rhs, this->y * rhs); }
		v2d_generic  operator *  (const v2d_generic& rhs) const { return v2d_generic(this->x * rhs.x, this->y * rhs.y); }
		v2d_generic  operator /  (const T& rhs)           const { return v2d_generic(this->x / rhs, this->y / rhs); }
		v2d_generic  operator /  (const v2d_generic& rhs) const { return v2d_generic(this->x / rhs.x, this->y / rhs.y); }
		v2d_generic& operator += (const v2d_generic& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
		v2d_generic& operator -= (const v2d_generic& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
		v2d_generic& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
		v2d_generic& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
		v2d_generic& operator *= (const v2d_generic& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
		v2d_generic& operator /= (const v2d_generic& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }
		v2d_generic  operator +  () const { return { +x, +y }; }
		v2d_generic  operator -  () const { return { -x, -y }; }
		bool operator == (const v2d_generic& rhs) const { return (this->x == rhs.x && this->y == rhs.y); }
		bool operator != (const v2d_generic& rhs) const { return (this->x != rhs.x || this->y != rhs.y); }
		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const v2d_generic& rhs) { os << rhs.str(); return os; }
		operator v2d_generic<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
		operator v2d_generic<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
		operator v2d_generic<double>() const { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
	};

	template<class T> inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (float)rhs.x), (T)(lhs * (float)rhs.y)); }
	template<class T> inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (double)rhs.x), (T)(lhs * (double)rhs.y)); }
	template<class T> inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs * (int)rhs.x), (T)(lhs * (int)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (float)rhs.x), (T)(lhs / (float)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (double)rhs.x), (T)(lhs / (double)rhs.y)); }
	template<class T> inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs)
	{ return v2d_generic<T>((T)(lhs / (int)rhs.x), (T)(lhs / (int)rhs.y)); }

	template<class T, class U> inline bool operator < (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{ return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x); }
	template<class T, class U> inline bool operator > (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{ return lhs.y > rhs.y || (lhs.y == rhs.y && lhs.x > rhs.x); }

	typedef v2d_generic<int32_t> vi2d;
	typedef v2d_generic<uint32_t> vu2d;
	typedef v2d_generic<float> vf2d;
	typedef v2d_generic<double> vd2d;
#endif


	// O------------------------------------------------------------------------------O
	// | alo::ResourcePack - A virtual scrambled filesystem to pack your assets into  |
	// O------------------------------------------------------------------------------O
	struct ResourceBuffer : public std::streambuf
	{
		ResourceBuffer(std::ifstream& ifs, uint32_t offset, uint32_t size);
		std::vector<char> vMemory;
	};

	class ResourcePack : public std::streambuf
	{
	public:
		ResourcePack();
		~ResourcePack();
		bool AddFile(const std::string& sFile);
		bool LoadPack(const std::string& sFile, const std::string& sKey);
		bool SavePack(const std::string& sFile, const std::string& sKey);
		ResourceBuffer GetFileBuffer(const std::string& sFile);
		bool Loaded();
	private:
		struct sResourceFile { uint32_t nSize; uint32_t nOffset; };
		std::map<std::string, sResourceFile> mapFiles;
		std::ifstream baseFile;
		std::vector<char> scramble(const std::vector<char>& data, const std::string& key);
		std::string makeposix(const std::string& path);
	};


	class ImageLoader
	{
	public:
		ImageLoader() = default;
		virtual ~ImageLoader() = default;
		virtual alo::rcode LoadImageResource(alo::Sprite* spr, const std::string& sImageFile, alo::ResourcePack* pack) = 0;
		virtual alo::rcode SaveImageResource(alo::Sprite* spr, const std::string& sImageFile) = 0;
	};


	// O------------------------------------------------------------------------------O
	// | alo::Sprite - An image represented by a 2D array of alo::Pixel               |
	// O------------------------------------------------------------------------------O
	class Sprite
	{
	public:
		Sprite();
		Sprite(const std::string& sImageFile, alo::ResourcePack* pack = nullptr);
		Sprite(int32_t w, int32_t h);
		Sprite(const alo::Sprite&) = delete;
		~Sprite();

	public:
		alo::rcode LoadFromFile(const std::string& sImageFile, alo::ResourcePack* pack = nullptr);

	public:
		int32_t width = 0;
		int32_t height = 0;
		enum Mode { NORMAL, PERIODIC, CLAMP };
		enum Flip { NONE = 0, HORIZ = 1, VERT = 2 };

	public:
		void SetSampleMode(alo::Sprite::Mode mode = alo::Sprite::Mode::NORMAL);
		Pixel GetPixel(int32_t x, int32_t y) const;
		bool  SetPixel(int32_t x, int32_t y, Pixel p);
		Pixel GetPixel(const alo::vi2d& a) const;
		bool  SetPixel(const alo::vi2d& a, Pixel p);
		Pixel Sample(float x, float y) const;
		Pixel SampleBL(float u, float v) const;
		Pixel* GetData();
		alo::Sprite* Duplicate();
		alo::Sprite* Duplicate(const alo::vi2d& vPos, const alo::vi2d& vSize);
		alo::vi2d Size() const;
		std::vector<alo::Pixel> pColData;
		Mode modeSample = Mode::NORMAL;

		static std::unique_ptr<alo::ImageLoader> loader;
	};

	// O------------------------------------------------------------------------------O
	// | alo::Decal - A GPU resident storage of an alo::Sprite                        |
	// O------------------------------------------------------------------------------O
	class Decal
	{
	public:
		Decal(alo::Sprite* spr, bool filter = false, bool clamp = true);
		Decal(const uint32_t nExistingTextureResource, alo::Sprite* spr);
		virtual ~Decal();
		void Update();
		void UpdateSprite();

	public: // But dont touch
		int32_t id = -1;
		alo::Sprite* sprite = nullptr;
		alo::vf2d vUVScale = { 1.0f, 1.0f };
	};

	enum class DecalMode
	{
		NORMAL,
		ADDITIVE,
		MULTIPLICATIVE,
		STENCIL,
		ILLUMINATE,
		WIREFRAME,
		MODEL3D,
	};

	enum class DecalStructure
	{
		LINE,
		FAN,
		STRIP,
		LIST
	};

	// O------------------------------------------------------------------------------O
	// | alo::Renderable - Convenience class to keep a sprite and decal together      |
	// O------------------------------------------------------------------------------O
	class Renderable
	{
	public:
		Renderable() = default;		
		Renderable(Renderable&& r) : pSprite(std::move(r.pSprite)), pDecal(std::move(r.pDecal)) {}		
		Renderable(const Renderable&) = delete;
		alo::rcode Load(const std::string& sFile, ResourcePack* pack = nullptr, bool filter = false, bool clamp = true);
		void Create(uint32_t width, uint32_t height, bool filter = false, bool clamp = true);
		alo::Decal* Decal() const;
		alo::Sprite* Sprite() const;

	private:
		std::unique_ptr<alo::Sprite> pSprite = nullptr;
		std::unique_ptr<alo::Decal> pDecal = nullptr;
	};


	// O------------------------------------------------------------------------------O
	// | Auxilliary components internal to engine                                     |
	// O------------------------------------------------------------------------------O

	struct DecalInstance
	{
		alo::Decal* decal = nullptr;
		std::vector<alo::vf2d> pos;
		std::vector<alo::vf2d> uv;
		std::vector<float> w;
		std::vector<alo::Pixel> tint;
		alo::DecalMode mode = alo::DecalMode::NORMAL;
		alo::DecalStructure structure = alo::DecalStructure::FAN;
		uint32_t points = 0;
	};

	struct LayerDesc
	{
		alo::vf2d vOffset = { 0, 0 };
		alo::vf2d vScale = { 1, 1 };
		bool bShow = false;
		bool bUpdate = false;
		alo::Renderable pDrawTarget;
		uint32_t nResID = 0;
		std::vector<DecalInstance> vecDecalInstance;
		alo::Pixel tint = alo::WHITE;
		std::function<void()> funcHook = nullptr;
	};

	class Renderer
	{
	public:
		virtual ~Renderer() = default;
		virtual void       PrepareDevice() = 0;
		virtual alo::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) = 0;
		virtual alo::rcode DestroyDevice() = 0;
		virtual void       DisplayFrame() = 0;
		virtual void       PrepareDrawing() = 0;
		virtual void	   SetDecalMode(const alo::DecalMode& mode) = 0;
		virtual void       DrawLayerQuad(const alo::vf2d& offset, const alo::vf2d& scale, const alo::Pixel tint) = 0;
		virtual void       DrawDecal(const alo::DecalInstance& decal) = 0;
		virtual uint32_t   CreateTexture(const uint32_t width, const uint32_t height, const bool filtered = false, const bool clamp = true) = 0;
		virtual void       UpdateTexture(uint32_t id, alo::Sprite* spr) = 0;
		virtual void       ReadTexture(uint32_t id, alo::Sprite* spr) = 0;
		virtual uint32_t   DeleteTexture(const uint32_t id) = 0;
		virtual void       ApplyTexture(uint32_t id) = 0;
		virtual void       UpdateViewport(const alo::vi2d& pos, const alo::vi2d& size) = 0;
		virtual void       ClearBuffer(alo::Pixel p, bool bDepth) = 0;
		static alo::GameEngine* ptrGE;
	};

	class Platform
	{
	public:
		virtual ~Platform() = default;
		virtual alo::rcode ApplicationStartUp() = 0;
		virtual alo::rcode ApplicationCleanUp() = 0;
		virtual alo::rcode ThreadStartUp() = 0;
		virtual alo::rcode ThreadCleanUp() = 0;
		virtual alo::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const alo::vi2d& vViewPos, const alo::vi2d& vViewSize) = 0;
		virtual alo::rcode CreateWindowPane(const alo::vi2d& vWindowPos, alo::vi2d& vWindowSize, bool bFullScreen) = 0;
		virtual alo::rcode SetWindowTitle(const std::string& s) = 0;
		virtual alo::rcode StartSystemEventLoop() = 0;
		virtual alo::rcode HandleSystemEvent() = 0;
		static alo::GameEngine* ptrGE;
	};

	class GEX;

	// The Static Twins (plus one)
	static std::unique_ptr<Renderer> renderer;
	static std::unique_ptr<Platform> platform;
	static std::map<size_t, uint8_t> mapKeys;

	// O------------------------------------------------------------------------------O
	// | alo::GameEngine - The main BASE class for your application                   |
	// O------------------------------------------------------------------------------O
	class GameEngine
	{
	public:
		GameEngine();
		virtual ~GameEngine();
	public:
		alo::rcode Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h,
			bool full_screen = false, bool vsync = false, bool cohesion = false);
		alo::rcode Start();

	public: // User Override Interfaces
		// Called once on application startup, use to load your resources
		virtual bool OnUserCreate();
		// Called every frame, and provides you with a time per frame value
		virtual bool OnUserUpdate(float fElapsedTime);
		// Called once on application termination, so you can be one clean coder
		virtual bool OnUserDestroy();

		// Called when a text entry is confirmed with "enter" key
		virtual void OnTextEntryComplete(const std::string& sText);
		// Called when a console command is executed
		virtual bool OnConsoleCommand(const std::string& sCommand);

	public: // Hardware Interfaces
		// Returns true if window is currently in focus
		bool IsFocused() const;
		// Get the state of a specific keyboard button
		HWButton GetKey(Key k) const;
		// Get the state of a specific mouse button
		HWButton GetMouse(uint32_t b) const;
		// Get Mouse X coordinate in "pixel" space
		int32_t GetMouseX() const;
		// Get Mouse Y coordinate in "pixel" space
		int32_t GetMouseY() const;
		// Get Mouse Wheel Delta
		int32_t GetMouseWheel() const;
		// Get the mouse in window space
		const alo::vi2d& GetWindowMouse() const;
		// Gets the mouse as a vector to keep Tarriest happy
		const alo::vi2d& GetMousePos() const;

		static const std::map<size_t, uint8_t>& GetKeyMap() { return mapKeys; }

	public: // Utility
		// Returns the width of the screen in "pixels"
		int32_t ScreenWidth() const;
		// Returns the height of the screen in "pixels"
		int32_t ScreenHeight() const;
		// Returns the width of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetWidth() const;
		// Returns the height of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetHeight() const;
		// Returns the currently active draw target
		alo::Sprite* GetDrawTarget() const;
		// Resize the primary screen sprite
		void SetScreenSize(int w, int h);
		// Specify which Sprite should be the target of drawing functions, use nullptr
		// to specify the primary screen
		void SetDrawTarget(Sprite* target);
		// Gets the current Frames Per Second
		uint32_t GetFPS() const;
		// Gets last update of elapsed time
		float GetElapsedTime() const;
		// Gets Actual Window size
		const alo::vi2d& GetWindowSize() const;
		// Gets pixel scale
		const alo::vi2d& GetPixelSize() const;
		// Gets actual pixel scale
		const alo::vi2d& GetScreenPixelSize() const;
		// Gets "screen" size
		const alo::vi2d& GetScreenSize() const;

	public: // CONFIGURATION ROUTINES
		// Layer targeting functions
		void SetDrawTarget(uint8_t layer, bool bDirty = true);
		void EnableLayer(uint8_t layer, bool b);
		void SetLayerOffset(uint8_t layer, const alo::vf2d& offset);
		void SetLayerOffset(uint8_t layer, float x, float y);
		void SetLayerScale(uint8_t layer, const alo::vf2d& scale);
		void SetLayerScale(uint8_t layer, float x, float y);
		void SetLayerTint(uint8_t layer, const alo::Pixel& tint);
		void SetLayerCustomRenderFunction(uint8_t layer, std::function<void()> f);

		std::vector<LayerDesc>& GetLayers();
		uint32_t CreateLayer();

		// Change the pixel mode for different optimisations
		// alo::Pixel::NORMAL = No transparency
		// alo::Pixel::MASK   = Transparent if alpha is < 255
		// alo::Pixel::ALPHA  = Full transparency
		void SetPixelMode(Pixel::Mode m);
		Pixel::Mode GetPixelMode();
		// Use a custom blend function
		void SetPixelMode(std::function<alo::Pixel(const int x, const int y, const alo::Pixel& pSource, const alo::Pixel& pDest)> pixelMode);
		// Change the blend factor from between 0.0f to 1.0f;
		void SetPixelBlend(float fBlend);



	public: // DRAWING ROUTINES
		// Draws a single Pixel
		virtual bool Draw(int32_t x, int32_t y, Pixel p = alo::WHITE);
		bool Draw(const alo::vi2d& pos, Pixel p = alo::WHITE);
		// Draws a line from (x1,y1) to (x2,y2)
		void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = alo::WHITE, uint32_t pattern = 0xFFFFFFFF);
		void DrawLine(const alo::vi2d& pos1, const alo::vi2d& pos2, Pixel p = alo::WHITE, uint32_t pattern = 0xFFFFFFFF);
		// Draws a circle located at (x,y) with radius
		void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p = alo::WHITE, uint8_t mask = 0xFF);
		void DrawCircle(const alo::vi2d& pos, int32_t radius, Pixel p = alo::WHITE, uint8_t mask = 0xFF);
		// Fills a circle located at (x,y) with radius
		void FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p = alo::WHITE);
		void FillCircle(const alo::vi2d& pos, int32_t radius, Pixel p = alo::WHITE);
		// Draws a rectangle at (x,y) to (x+w,y+h)
		void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = alo::WHITE);
		void DrawRect(const alo::vi2d& pos, const alo::vi2d& size, Pixel p = alo::WHITE);
		// Fills a rectangle at (x,y) to (x+w,y+h)
		void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = alo::WHITE);
		void FillRect(const alo::vi2d& pos, const alo::vi2d& size, Pixel p = alo::WHITE);
		// Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = alo::WHITE);
		void DrawTriangle(const alo::vi2d& pos1, const alo::vi2d& pos2, const alo::vi2d& pos3, Pixel p = alo::WHITE);
		// Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = alo::WHITE);
		void FillTriangle(const alo::vi2d& pos1, const alo::vi2d& pos2, const alo::vi2d& pos3, Pixel p = alo::WHITE);
		// Draws an entire sprite at location (x,y)
		void DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale = 1, uint8_t flip = alo::Sprite::NONE);
		void DrawSprite(const alo::vi2d& pos, Sprite* sprite, uint32_t scale = 1, uint8_t flip = alo::Sprite::NONE);
		// Draws an area of a sprite at location (x,y), where the
		// selected area is (ox,oy) to (ox+w,oy+h)
		void DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1, uint8_t flip = alo::Sprite::NONE);
		void DrawPartialSprite(const alo::vi2d& pos, Sprite* sprite, const alo::vi2d& sourcepos, const alo::vi2d& size, uint32_t scale = 1, uint8_t flip = alo::Sprite::NONE);
		// Draws a single line of text - traditional monospaced
		void DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col = alo::WHITE, uint32_t scale = 1);
		void DrawString(const alo::vi2d& pos, const std::string& sText, Pixel col = alo::WHITE, uint32_t scale = 1);
		alo::vi2d GetTextSize(const std::string& s);
		// Draws a single line of text - non-monospaced
		void DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col = alo::WHITE, uint32_t scale = 1);
		void DrawStringProp(const alo::vi2d& pos, const std::string& sText, Pixel col = alo::WHITE, uint32_t scale = 1);
		alo::vi2d GetTextSizeProp(const std::string& s);

		// Decal Quad functions
		void SetDecalMode(const alo::DecalMode& mode);
		void SetDecalStructure(const alo::DecalStructure& structure);
		// Draws a whole decal, with optional scale and tinting
		void DrawDecal(const alo::vf2d& pos, alo::Decal* decal, const alo::vf2d& scale = { 1.0f,1.0f }, const alo::Pixel& tint = alo::WHITE);
		// Draws a region of a decal, with optional scale and tinting
		void DrawPartialDecal(const alo::vf2d& pos, alo::Decal* decal, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::vf2d& scale = { 1.0f,1.0f }, const alo::Pixel& tint = alo::WHITE);
		void DrawPartialDecal(const alo::vf2d& pos, const alo::vf2d& size, alo::Decal* decal, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint = alo::WHITE);
		// Draws fully user controlled 4 vertices, pos(pixels), uv(pixels), colours
		void DrawExplicitDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::vf2d* uv, const alo::Pixel* col, uint32_t elements = 4);
		// Draws a decal with 4 arbitrary points, warping the texture to look "correct"
		void DrawWarpedDecal(alo::Decal* decal, const alo::vf2d(&pos)[4], const alo::Pixel& tint = alo::WHITE);
		void DrawWarpedDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::Pixel& tint = alo::WHITE);
		void DrawWarpedDecal(alo::Decal* decal, const std::array<alo::vf2d, 4>& pos, const alo::Pixel& tint = alo::WHITE);
		// As above, but you can specify a region of a decal source sprite
		void DrawPartialWarpedDecal(alo::Decal* decal, const alo::vf2d(&pos)[4], const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint = alo::WHITE);
		void DrawPartialWarpedDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint = alo::WHITE);
		void DrawPartialWarpedDecal(alo::Decal* decal, const std::array<alo::vf2d, 4>& pos, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint = alo::WHITE);
		// Draws a decal rotated to specified angle, wit point of rotation offset
		void DrawRotatedDecal(const alo::vf2d& pos, alo::Decal* decal, const float fAngle, const alo::vf2d& center = { 0.0f, 0.0f }, const alo::vf2d& scale = { 1.0f,1.0f }, const alo::Pixel& tint = alo::WHITE);
		void DrawPartialRotatedDecal(const alo::vf2d& pos, alo::Decal* decal, const float fAngle, const alo::vf2d& center, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::vf2d& scale = { 1.0f, 1.0f }, const alo::Pixel& tint = alo::WHITE);
		// Draws a multiline string as a decal, with tiniting and scaling
		void DrawStringDecal(const alo::vf2d& pos, const std::string& sText, const Pixel col = alo::WHITE, const alo::vf2d& scale = { 1.0f, 1.0f });
		void DrawStringPropDecal(const alo::vf2d& pos, const std::string& sText, const Pixel col = alo::WHITE, const alo::vf2d& scale = { 1.0f, 1.0f });
		// Draws a single shaded filled rectangle as a decal
		void DrawRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel col = alo::WHITE);
		void FillRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel col = alo::WHITE);
		// Draws a corner shaded rectangle as a decal
		void GradientFillRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel colTL, const alo::Pixel colBL, const alo::Pixel colBR, const alo::Pixel colTR);
		// Draws an arbitrary convex textured polygon using GPU
		void DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<alo::vf2d>& uv, const alo::Pixel tint = alo::WHITE);
		void DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<float>& depth, const std::vector<alo::vf2d>& uv, const alo::Pixel tint = alo::WHITE);
		void DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<alo::vf2d>& uv, const std::vector<alo::Pixel>& tint);

		// Draws a line in Decal Space
		void DrawLineDecal(const alo::vf2d& pos1, const alo::vf2d& pos2, Pixel p = alo::WHITE);
		void DrawRotatedStringDecal(const alo::vf2d& pos, const std::string& sText, const float fAngle, const alo::vf2d& center = { 0.0f, 0.0f }, const alo::Pixel col = alo::WHITE, const alo::vf2d& scale = { 1.0f, 1.0f });
		void DrawRotatedStringPropDecal(const alo::vf2d& pos, const std::string& sText, const float fAngle, const alo::vf2d& center = { 0.0f, 0.0f }, const alo::Pixel col = alo::WHITE, const alo::vf2d& scale = { 1.0f, 1.0f });
		// Clears entire draw target to Pixel
		void Clear(Pixel p);
		// Clears the rendering back buffer
		void ClearBuffer(Pixel p, bool bDepth = true);
		// Returns the font image
		alo::Sprite* GetFontSprite();

		// Clip a line segment to visible area
		bool ClipLineToScreen(alo::vi2d& in_p1, alo::vi2d& in_p2);

		// Dont allow PGE to mark layers as dirty, so pixel graphics don't update
		void EnablePixelTransfer(const bool bEnable = true);

		// Command Console Routines
		void ConsoleShow(const alo::Key &keyExit, bool bSuspendTime = true);
		bool IsConsoleShowing() const;
		void ConsoleClear();
		std::stringstream& ConsoleOut();
		void ConsoleCaptureStdOut(const bool bCapture);

		// Text Entry Routines
		void TextEntryEnable(const bool bEnable, const std::string& sText = "");
		std::string TextEntryGetString() const;
		int32_t TextEntryGetCursor() const;
		bool IsTextEntryEnabled() const;



	private:
		void UpdateTextEntry();
		void UpdateConsole();

	
	public: // Branding
		std::string sAppName;

	private: // Inner mysterious workings
		alo::Sprite*     pDrawTarget = nullptr;
		Pixel::Mode	nPixelMode = Pixel::NORMAL;
		float		fBlendFactor = 1.0f;
		alo::vi2d	vScreenSize = { 256, 240 };
		alo::vf2d	vInvScreenSize = { 1.0f / 256.0f, 1.0f / 240.0f };
		alo::vi2d	vPixelSize = { 4, 4 };
		alo::vi2d   vScreenPixelSize = { 4, 4 };
		alo::vi2d	vMousePos = { 0, 0 };
		int32_t		nMouseWheelDelta = 0;
		alo::vi2d	vMousePosCache = { 0, 0 };
		alo::vi2d   vMouseWindowPos = { 0, 0 };
		int32_t		nMouseWheelDeltaCache = 0;
		alo::vi2d	vWindowSize = { 0, 0 };
		alo::vi2d	vViewPos = { 0, 0 };
		alo::vi2d	vViewSize = { 0,0 };
		bool		bFullScreen = false;
		alo::vf2d	vPixel = { 1.0f, 1.0f };
		bool		bHasInputFocus = false;
		bool		bHasMouseFocus = false;
		bool		bEnableVSYNC = false;
		float		fFrameTimer = 1.0f;
		float		fLastElapsed = 0.0f;
		int			nFrameCount = 0;		
		bool bSuspendTextureTransfer = false;
		Renderable  fontRenderable;
		std::vector<LayerDesc> vLayers;
		uint8_t		nTargetLayer = 0;
		uint32_t	nLastFPS = 0;
		bool        bPixelCohesion = false;
		DecalMode   nDecalMode = DecalMode::NORMAL;
		DecalStructure nDecalStructure = DecalStructure::FAN;
		std::function<alo::Pixel(const int x, const int y, const alo::Pixel&, const alo::Pixel&)> funcPixelMode;
		std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;
		std::vector<alo::vi2d> vFontSpacing;

		// Command Console Specific
		bool bConsoleShow = false;
		bool bConsoleSuspendTime = false;
		alo::Key keyConsoleExit = alo::Key::F1;
		std::stringstream ssConsoleOutput;
		std::streambuf* sbufOldCout = nullptr;
		alo::vi2d vConsoleSize;
		alo::vi2d vConsoleCursor = { 0,0 };
		alo::vf2d vConsoleCharacterScale = { 1.0f, 2.0f };
		std::vector<std::string> sConsoleLines;
		std::list<std::string> sCommandHistory;
		std::list<std::string>::iterator sCommandHistoryIt;

		// Text Entry Specific
		bool bTextEntryEnable = false;
		std::string sTextEntryString = "";
		int32_t nTextEntryCursor = 0;
		std::vector<std::tuple<alo::Key, std::string, std::string>> vKeyboardMap;



		// State of keyboard		
		bool		pKeyNewState[256] = { 0 };
		bool		pKeyOldState[256] = { 0 };
		HWButton	pKeyboardState[256] = { 0 };

		// State of mouse
		bool		pMouseNewState[nMouseButtons] = { 0 };
		bool		pMouseOldState[nMouseButtons] = { 0 };
		HWButton	pMouseState[nMouseButtons] = { 0 };

		// The main engine thread
		void		EngineThread();


		// If anything sets this flag to false, the engine
		// "should" shut down gracefully
		static std::atomic<bool> bAtomActive;

	public:
		// "Break In" Functions
		void alo_UpdateMouse(int32_t x, int32_t y);
		void alo_UpdateMouseWheel(int32_t delta);
		void alo_UpdateWindowSize(int32_t x, int32_t y);
		void alo_UpdateViewport();
		void alo_ConstructFontSheet();
		void alo_CoreUpdate();
		void alo_PrepareEngine();
		void alo_UpdateMouseState(int32_t button, bool state);
		void alo_UpdateKeyState(int32_t key, bool state);
		void alo_UpdateMouseFocus(bool state);
		void alo_UpdateKeyFocus(bool state);
		void alo_Terminate();
		void alo_Reanimate();
		bool alo_IsRunning();

		// At the very end of this file, chooses which
		// components to compile
		virtual void alo_ConfigureSystem();

		// NOTE: Items Here are to be deprecated, I have left them in for now
		// in case you are using them, but they will be removed.
		// alo::vf2d	vSubPixelOffset = { 0.0f, 0.0f };

	public: // GEX Stuff
		friend class GEX;
		void gex_Register(alo::GEX* gex);

	private:
		std::vector<alo::GEX*> vExtensions;
	};



	// O------------------------------------------------------------------------------O
	// |  GE EXTENSION BASE CLASS - Permits access to PGE functions from extension    |
	// O------------------------------------------------------------------------------O
	class GEX
	{
		friend class alo::GameEngine;
	public:
		GEX(bool bHook = false);

	protected:
		virtual void OnBeforeUserCreate();
		virtual void OnAfterUserCreate();
		virtual bool OnBeforeUserUpdate(float &fElapsedTime);
		virtual void OnAfterUserUpdate(float fElapsedTime);

	protected:
		static GameEngine* ge;
	};
}

#pragma endregion

#endif // ALO_GE_DEF


// O------------------------------------------------------------------------------O
// |  START OF ALO_GE_APPLICATION                                                 |
// O------------------------------------------------------------------------------O
#ifdef ALO_GE_APPLICATION
#undef ALO_GE_APPLICATION

// O------------------------------------------------------------------------------O
// | aloGameEngine INTERFACE IMPLEMENTATION (CORE)                                |
// | Note: The core implementation is platform independent                        |
// O------------------------------------------------------------------------------O
#pragma region ge_implementation
namespace alo
{
	// O------------------------------------------------------------------------------O
	// | alo::Pixel IMPLEMENTATION                                                    |
	// O------------------------------------------------------------------------------O
	Pixel::Pixel()
	{ r = 0; g = 0; b = 0; a = nDefaultAlpha; }

	Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{ n = red | (green << 8) | (blue << 16) | (alpha << 24); } // Thanks jarekpelczar 

	Pixel::Pixel(uint32_t p)
	{ n = p; }

	bool Pixel::operator==(const Pixel& p) const
	{ return n == p.n; }

	bool Pixel::operator!=(const Pixel& p) const
	{ return n != p.n; }

	Pixel  Pixel::operator * (const float i) const
	{
		float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
		float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
		float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
		return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
	}

	Pixel  Pixel::operator / (const float i) const
	{
		float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
		float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
		float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
		return Pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
	}

	Pixel& Pixel::operator *=(const float i)
	{
		this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
		this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
		this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
		return *this;
	}

	Pixel& Pixel::operator /=(const float i)
	{
		this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
		this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
		this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
		return *this;
	}

	Pixel  Pixel::operator + (const Pixel& p) const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel  Pixel::operator - (const Pixel& p) const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel& Pixel::operator += (const Pixel& p)
	{
		this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
		this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
		this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
		return *this;
	}

	Pixel& Pixel::operator -= (const Pixel& p) // Thanks Au Lit
	{
		this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
		this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
		this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
		return *this;
	}

	Pixel Pixel::inv() const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel PixelF(float red, float green, float blue, float alpha)
	{ return Pixel(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f)); }

	Pixel PixelLerp(const alo::Pixel& p1, const alo::Pixel& p2, float t)
	{ return (p2 * t) + p1 * (1.0f - t); }

	// O------------------------------------------------------------------------------O
	// | alo::Sprite IMPLEMENTATION                                                   |
	// O------------------------------------------------------------------------------O
	Sprite::Sprite()
	{ width = 0; height = 0; }

	Sprite::Sprite(const std::string& sImageFile, alo::ResourcePack* pack)
	{ LoadFromFile(sImageFile, pack); }

	Sprite::Sprite(int32_t w, int32_t h)
	{		
		width = w;		height = h;
		pColData.resize(width * height);
		pColData.resize(width * height, nDefaultPixel);
	}

	Sprite::~Sprite()
	{ pColData.clear();	}

	void Sprite::SetSampleMode(alo::Sprite::Mode mode)
	{ modeSample = mode; }

	Pixel Sprite::GetPixel(const alo::vi2d& a) const
	{ return GetPixel(a.x, a.y); }

	bool Sprite::SetPixel(const alo::vi2d& a, Pixel p)
	{ return SetPixel(a.x, a.y, p); }

	Pixel Sprite::GetPixel(int32_t x, int32_t y) const
	{
		if (modeSample == alo::Sprite::Mode::NORMAL)
		{
			if (x >= 0 && x < width && y >= 0 && y < height)
				return pColData[y * width + x];
			else
				return Pixel(0, 0, 0, 0);
		}
		else
		{
			if (modeSample == alo::Sprite::Mode::PERIODIC)
				return pColData[abs(y % height) * width + abs(x % width)];
			else
				return pColData[std::max(0, std::min(y, height-1)) * width + std::max(0, std::min(x, width-1))];
		}
	}

	bool Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			pColData[y * width + x] = p;
			return true;
		}
		else
			return false;
	}

	Pixel Sprite::Sample(float x, float y) const
	{
		int32_t sx = std::min((int32_t)((x * (float)width)), width - 1);
		int32_t sy = std::min((int32_t)((y * (float)height)), height - 1);
		return GetPixel(sx, sy);
	}

	Pixel Sprite::SampleBL(float u, float v) const
	{
		u = u * width - 0.5f;
		v = v * height - 0.5f;
		int x = (int)floor(u); // cast to int rounds toward zero, not downward
		int y = (int)floor(v); 
		float u_ratio = u - x;
		float v_ratio = v - y;
		float u_opposite = 1 - u_ratio;
		float v_opposite = 1 - v_ratio;

		alo::Pixel p1 = GetPixel(std::max(x, 0), std::max(y, 0));
		alo::Pixel p2 = GetPixel(std::min(x + 1, (int)width - 1), std::max(y, 0));
		alo::Pixel p3 = GetPixel(std::max(x, 0), std::min(y + 1, (int)height - 1));
		alo::Pixel p4 = GetPixel(std::min(x + 1, (int)width - 1), std::min(y + 1, (int)height - 1));

		return alo::Pixel(
			(uint8_t)((p1.r * u_opposite + p2.r * u_ratio) * v_opposite + (p3.r * u_opposite + p4.r * u_ratio) * v_ratio),
			(uint8_t)((p1.g * u_opposite + p2.g * u_ratio) * v_opposite + (p3.g * u_opposite + p4.g * u_ratio) * v_ratio),
			(uint8_t)((p1.b * u_opposite + p2.b * u_ratio) * v_opposite + (p3.b * u_opposite + p4.b * u_ratio) * v_ratio));
	}

	Pixel* Sprite::GetData()
	{ return pColData.data(); }


	alo::rcode Sprite::LoadFromFile(const std::string& sImageFile, alo::ResourcePack* pack)
	{
		UNUSED(pack);
		return loader->LoadImageResource(this, sImageFile, pack);
	}

	alo::Sprite* Sprite::Duplicate()
	{
		alo::Sprite* spr = new alo::Sprite(width, height);
		std::memcpy(spr->GetData(), GetData(), width * height * sizeof(alo::Pixel));
		spr->modeSample = modeSample;
		return spr;
	}

	alo::Sprite* Sprite::Duplicate(const alo::vi2d& vPos, const alo::vi2d& vSize)
	{
		alo::Sprite* spr = new alo::Sprite(vSize.x, vSize.y);
		for (int y = 0; y < vSize.y; y++)
			for (int x = 0; x < vSize.x; x++)
				spr->SetPixel(x, y, GetPixel(vPos.x + x, vPos.y + y));
		return spr;
	}

	alo::vi2d alo::Sprite::Size() const
	{
		return { width, height };
	}

	// O------------------------------------------------------------------------------O
	// | alo::Decal IMPLEMENTATION                                                    |
	// O------------------------------------------------------------------------------O
	Decal::Decal(alo::Sprite* spr, bool filter, bool clamp)
	{
		id = -1;
		if (spr == nullptr) return;
		sprite = spr;
		id = renderer->CreateTexture(sprite->width, sprite->height, filter, clamp);
		Update();
	}

	Decal::Decal(const uint32_t nExistingTextureResource, alo::Sprite* spr)
	{
		if (spr == nullptr) return;
		id = nExistingTextureResource;
	}

	void Decal::Update()
	{
		if (sprite == nullptr) return;
		vUVScale = { 1.0f / float(sprite->width), 1.0f / float(sprite->height) };
		renderer->ApplyTexture(id);
		renderer->UpdateTexture(id, sprite);
	}

	void Decal::UpdateSprite()
	{
		if (sprite == nullptr) return;
		renderer->ApplyTexture(id);
		renderer->ReadTexture(id, sprite);
	}

	Decal::~Decal()
	{
		if (id != -1)
		{
			renderer->DeleteTexture(id);
			id = -1;
		}
	}

	void Renderable::Create(uint32_t width, uint32_t height, bool filter, bool clamp)
	{
		pSprite = std::make_unique<alo::Sprite>(width, height);
		pDecal = std::make_unique<alo::Decal>(pSprite.get(), filter, clamp);
	}

	alo::rcode Renderable::Load(const std::string& sFile, ResourcePack* pack, bool filter, bool clamp)
	{
		pSprite = std::make_unique<alo::Sprite>();
		if (pSprite->LoadFromFile(sFile, pack) == alo::rcode::OK)
		{
			pDecal = std::make_unique<alo::Decal>(pSprite.get(), filter, clamp);
			return alo::rcode::OK;
		}
		else
		{
			pSprite.release();
			pSprite = nullptr;
			return alo::rcode::NO_FILE;
		}
	}

	alo::Decal* Renderable::Decal() const
	{ return pDecal.get(); }

	alo::Sprite* Renderable::Sprite() const
	{ return pSprite.get(); }

	// O------------------------------------------------------------------------------O
	// | alo::ResourcePack IMPLEMENTATION                                             |
	// O------------------------------------------------------------------------------O


	//=============================================================
	// Resource Packs - Allows you to store files in one large 
	// scrambled file 
	ResourceBuffer::ResourceBuffer(std::ifstream& ifs, uint32_t offset, uint32_t size)
	{
		vMemory.resize(size);
		ifs.seekg(offset); ifs.read(vMemory.data(), vMemory.size());
		setg(vMemory.data(), vMemory.data(), vMemory.data() + size);
	}

	ResourcePack::ResourcePack() { }
	ResourcePack::~ResourcePack() { baseFile.close(); }

	bool ResourcePack::AddFile(const std::string& sFile)
	{
		const std::string file = makeposix(sFile);

		if (_gfs::exists(file))
		{
			sResourceFile e;
			e.nSize = (uint32_t)_gfs::file_size(file);
			e.nOffset = 0; // Unknown at this stage			
			mapFiles[file] = e;
			return true;
		}
		return false;
	}

	bool ResourcePack::LoadPack(const std::string& sFile, const std::string& sKey)
	{
		// Open the resource file
		baseFile.open(sFile, std::ifstream::binary);
		if (!baseFile.is_open()) return false;

		// 1) Read Scrambled index
		uint32_t nIndexSize = 0;
		baseFile.read((char*)&nIndexSize, sizeof(uint32_t));

		std::vector<char> buffer(nIndexSize);
		for (uint32_t j = 0; j < nIndexSize; j++)
			buffer[j] = baseFile.get();

		std::vector<char> decoded = scramble(buffer, sKey);
		size_t pos = 0;
		auto read = [&decoded, &pos](char* dst, size_t size) {
			memcpy((void*)dst, (const void*)(decoded.data() + pos), size);
			pos += size;
		};

		auto get = [&read]() -> int { char c; read(&c, 1); return c; };

		// 2) Read Map
		uint32_t nMapEntries = 0;
		read((char*)&nMapEntries, sizeof(uint32_t));
		for (uint32_t i = 0; i < nMapEntries; i++)
		{
			uint32_t nFilePathSize = 0;
			read((char*)&nFilePathSize, sizeof(uint32_t));

			std::string sFileName(nFilePathSize, ' ');
			for (uint32_t j = 0; j < nFilePathSize; j++)
				sFileName[j] = get();

			sResourceFile e;
			read((char*)&e.nSize, sizeof(uint32_t));
			read((char*)&e.nOffset, sizeof(uint32_t));
			mapFiles[sFileName] = e;
		}

		// Don't close base file! we will provide a stream
		// pointer when the file is requested
		return true;
	}

	bool ResourcePack::SavePack(const std::string& sFile, const std::string& sKey)
	{
		// Create/Overwrite the resource file
		std::ofstream ofs(sFile, std::ofstream::binary);
		if (!ofs.is_open()) return false;

		// Iterate through map
		uint32_t nIndexSize = 0; // Unknown for now
		ofs.write((char*)&nIndexSize, sizeof(uint32_t));
		uint32_t nMapSize = uint32_t(mapFiles.size());
		ofs.write((char*)&nMapSize, sizeof(uint32_t));
		for (auto& e : mapFiles)
		{
			// Write the path of the file
			size_t nPathSize = e.first.size();
			ofs.write((char*)&nPathSize, sizeof(uint32_t));
			ofs.write(e.first.c_str(), nPathSize);

			// Write the file entry properties
			ofs.write((char*)&e.second.nSize, sizeof(uint32_t));
			ofs.write((char*)&e.second.nOffset, sizeof(uint32_t));
		}

		// 2) Write the individual Data
		std::streampos offset = ofs.tellp();
		nIndexSize = (uint32_t)offset;
		for (auto& e : mapFiles)
		{
			// Store beginning of file offset within resource pack file
			e.second.nOffset = (uint32_t)offset;

			// Load the file to be added
			std::vector<uint8_t> vBuffer(e.second.nSize);
			std::ifstream i(e.first, std::ifstream::binary);
			i.read((char*)vBuffer.data(), e.second.nSize);
			i.close();

			// Write the loaded file into resource pack file
			ofs.write((char*)vBuffer.data(), e.second.nSize);
			offset += e.second.nSize;
		}

		// 3) Scramble Index
		std::vector<char> stream;
		auto write = [&stream](const char* data, size_t size) {
			size_t sizeNow = stream.size();
			stream.resize(sizeNow + size);
			memcpy(stream.data() + sizeNow, data, size);
		};

		// Iterate through map
		write((char*)&nMapSize, sizeof(uint32_t));
		for (auto& e : mapFiles)
		{
			// Write the path of the file
			size_t nPathSize = e.first.size();
			write((char*)&nPathSize, sizeof(uint32_t));
			write(e.first.c_str(), nPathSize);

			// Write the file entry properties
			write((char*)&e.second.nSize, sizeof(uint32_t));
			write((char*)&e.second.nOffset, sizeof(uint32_t));
		}
		std::vector<char> sIndexString = scramble(stream, sKey);
		uint32_t nIndexStringLen = uint32_t(sIndexString.size());
		// 4) Rewrite Map (it has been updated with offsets now)
		// at start of file
		ofs.seekp(0, std::ios::beg);
		ofs.write((char*)&nIndexStringLen, sizeof(uint32_t));
		ofs.write(sIndexString.data(), nIndexStringLen);
		ofs.close();
		return true;
	}

	ResourceBuffer ResourcePack::GetFileBuffer(const std::string& sFile)
	{ return ResourceBuffer(baseFile, mapFiles[sFile].nOffset, mapFiles[sFile].nSize); }

	bool ResourcePack::Loaded()
	{ return baseFile.is_open(); }

	std::vector<char> ResourcePack::scramble(const std::vector<char>& data, const std::string& key)
	{
		if (key.empty()) return data;
		std::vector<char> o;
		size_t c = 0;
		for (auto s : data)	o.push_back(s ^ key[(c++) % key.size()]);
		return o;
	};

	std::string ResourcePack::makeposix(const std::string& path)
	{
		std::string o;
		for (auto s : path) o += std::string(1, s == '\\' ? '/' : s);
		return o;
	};

	// O------------------------------------------------------------------------------O
	// | alo::GameEngine IMPLEMENTATION                                               |
	// O------------------------------------------------------------------------------O
	GameEngine::GameEngine()
	{
		sAppName = "Undefined";
		alo::GEX::ge = this;

		// Bring in relevant Platform & Rendering systems depending
		// on compiler parameters
		alo_ConfigureSystem();
	}

	GameEngine::~GameEngine()
	{}


	alo::rcode GameEngine::Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h, bool full_screen, bool vsync, bool cohesion)
	{
		bPixelCohesion = cohesion;
		vScreenSize = { screen_w, screen_h };
		vInvScreenSize = { 1.0f / float(screen_w), 1.0f / float(screen_h) };
		vPixelSize = { pixel_w, pixel_h };
		vWindowSize = vScreenSize * vPixelSize;
		bFullScreen = full_screen;
		bEnableVSYNC = vsync;
		vPixel = 2.0f / vScreenSize;

		if (vPixelSize.x <= 0 || vPixelSize.y <= 0 || vScreenSize.x <= 0 || vScreenSize.y <= 0)
			return alo::FAIL;
		return alo::OK;
	}


	void GameEngine::SetScreenSize(int w, int h)
	{
		vScreenSize = { w, h };
		vInvScreenSize = { 1.0f / float(w), 1.0f / float(h) };
		for (auto& layer : vLayers)
		{
			layer.pDrawTarget.Create(vScreenSize.x, vScreenSize.y);
			layer.bUpdate = true;
		}
		SetDrawTarget(nullptr);
		renderer->ClearBuffer(alo::BLACK, true);
		renderer->DisplayFrame();
		renderer->ClearBuffer(alo::BLACK, true);
		renderer->UpdateViewport(vViewPos, vViewSize);
	}

#if !defined(PGE_USE_CUSTOM_START)
	alo::rcode GameEngine::Start()
	{
		if (platform->ApplicationStartUp() != alo::OK) return alo::FAIL;

		// Construct the window
		if (platform->CreateWindowPane({ 30,30 }, vWindowSize, bFullScreen) != alo::OK) return alo::FAIL;
		alo_UpdateWindowSize(vWindowSize.x, vWindowSize.y);

		// Start the thread
		bAtomActive = true;
		std::thread t = std::thread(&GameEngine::EngineThread, this);

		// Some implementations may form an event loop here
		platform->StartSystemEventLoop();

		// Wait for thread to be exited
		t.join();

		if (platform->ApplicationCleanUp() != alo::OK) return alo::FAIL;

		return alo::OK;
	}
#endif

	void GameEngine::SetDrawTarget(Sprite* target)
	{
		if (target)
		{
			pDrawTarget = target;
		}
		else
		{
			nTargetLayer = 0;
			pDrawTarget = vLayers[0].pDrawTarget.Sprite();
		}
	}

	void GameEngine::SetDrawTarget(uint8_t layer, bool bDirty)
	{
		if (layer < vLayers.size())
		{
			pDrawTarget = vLayers[layer].pDrawTarget.Sprite();
			vLayers[layer].bUpdate = bDirty;
			nTargetLayer = layer;
		}
	}

	void GameEngine::EnableLayer(uint8_t layer, bool b)
	{ if (layer < vLayers.size()) vLayers[layer].bShow = b; }

	void GameEngine::SetLayerOffset(uint8_t layer, const alo::vf2d& offset)
	{ SetLayerOffset(layer, offset.x, offset.y); }

	void GameEngine::SetLayerOffset(uint8_t layer, float x, float y)
	{ if (layer < vLayers.size()) vLayers[layer].vOffset = { x, y }; }

	void GameEngine::SetLayerScale(uint8_t layer, const alo::vf2d& scale)
	{ SetLayerScale(layer, scale.x, scale.y); }

	void GameEngine::SetLayerScale(uint8_t layer, float x, float y)
	{ if (layer < vLayers.size()) vLayers[layer].vScale = { x, y }; }

	void GameEngine::SetLayerTint(uint8_t layer, const alo::Pixel& tint)
	{ if (layer < vLayers.size()) vLayers[layer].tint = tint; }

	void GameEngine::SetLayerCustomRenderFunction(uint8_t layer, std::function<void()> f)
	{ if (layer < vLayers.size()) vLayers[layer].funcHook = f; }

	std::vector<LayerDesc>& GameEngine::GetLayers()
	{ return vLayers; }

	uint32_t GameEngine::CreateLayer()
	{
		LayerDesc ld;
		ld.pDrawTarget.Create(vScreenSize.x, vScreenSize.y);
		vLayers.push_back(std::move(ld));
		return uint32_t(vLayers.size()) - 1;
	}

	Sprite* GameEngine::GetDrawTarget() const
	{ return pDrawTarget; }

	int32_t GameEngine::GetDrawTargetWidth() const
	{
		if (pDrawTarget)
			return pDrawTarget->width;
		else
			return 0;
	}

	int32_t GameEngine::GetDrawTargetHeight() const
	{
		if (pDrawTarget)
			return pDrawTarget->height;
		else
			return 0;
	}

	uint32_t GameEngine::GetFPS() const
	{ return nLastFPS; }

	bool GameEngine::IsFocused() const
	{ return bHasInputFocus; }

	HWButton GameEngine::GetKey(Key k) const
	{ return pKeyboardState[k];	}

	HWButton GameEngine::GetMouse(uint32_t b) const
	{ return pMouseState[b]; }

	int32_t GameEngine::GetMouseX() const
	{ return vMousePos.x; }

	int32_t GameEngine::GetMouseY() const
	{ return vMousePos.y; }

	const alo::vi2d& GameEngine::GetMousePos() const
	{ return vMousePos; }

	int32_t GameEngine::GetMouseWheel() const
	{ return nMouseWheelDelta; }

	int32_t GameEngine::ScreenWidth() const
	{ return vScreenSize.x; }

	int32_t GameEngine::ScreenHeight() const
	{ return vScreenSize.y; }

	float GameEngine::GetElapsedTime() const
	{ return fLastElapsed; }

	const alo::vi2d& GameEngine::GetWindowSize() const
	{ return vWindowSize; }

	const alo::vi2d& GameEngine::GetPixelSize() const
	{ return vPixelSize; }

	const alo::vi2d& GameEngine::GetScreenPixelSize() const
	{ return vScreenPixelSize; }

	const alo::vi2d& GameEngine::GetScreenSize() const
	{ return vScreenSize;	}

	const alo::vi2d& GameEngine::GetWindowMouse() const
	{ return vMouseWindowPos; }

	bool GameEngine::Draw(const alo::vi2d& pos, Pixel p)
	{ return Draw(pos.x, pos.y, p); }

	// This is it, the critical function that plots a pixel
	bool GameEngine::Draw(int32_t x, int32_t y, Pixel p)
	{
		if (!pDrawTarget) return false;

		if (nPixelMode == Pixel::NORMAL)
		{
			return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::MASK)
		{
			if (p.a == 255)
				return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::ALPHA)
		{
			Pixel d = pDrawTarget->GetPixel(x, y);
			float a = (float)(p.a / 255.0f) * fBlendFactor;
			float c = 1.0f - a;
			float r = a * (float)p.r + c * (float)d.r;
			float g = a * (float)p.g + c * (float)d.g;
			float b = a * (float)p.b + c * (float)d.b;
			return pDrawTarget->SetPixel(x, y, Pixel((uint8_t)r, (uint8_t)g, (uint8_t)b/*, (uint8_t)(p.a * fBlendFactor)*/));
		}

		if (nPixelMode == Pixel::CUSTOM)
		{
			return pDrawTarget->SetPixel(x, y, funcPixelMode(x, y, p, pDrawTarget->GetPixel(x, y)));
		}

		return false;
	}


	void GameEngine::DrawLine(const alo::vi2d& pos1, const alo::vi2d& pos2, Pixel p, uint32_t pattern)
	{ DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p, pattern); }

	void GameEngine::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p, uint32_t pattern)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

		alo::vi2d p1(x1, y1), p2(x2, y2);
		//if (!ClipLineToScreen(p1, p2))
		//	return;
		x1 = p1.x; y1 = p1.y;
		x2 = p2.x; y2 = p2.y;

		// straight lines idea by gurkanctn
		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) std::swap(y1, y2);
			for (y = y1; y <= y2; y++) if (rol()) Draw(x1, y, p);
			return;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) std::swap(x1, x2);
			for (x = x1; x <= x2; x++) if (rol()) Draw(x, y1, p);
			return;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				if (rol()) Draw(x, y, p);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				if (rol()) Draw(x, y, p);
			}
		}
	}

	void GameEngine::DrawCircle(const alo::vi2d& pos, int32_t radius, Pixel p, uint8_t mask)
	{ DrawCircle(pos.x, pos.y, radius, p, mask); }

	void GameEngine::DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p, uint8_t mask)
	{ 
		if (radius < 0 || x < -radius || y < -radius || x - GetDrawTargetWidth() > radius || y - GetDrawTargetHeight() > radius)
			return;

		if (radius > 0)
		{
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;

			while (y0 >= x0) // only formulate 1/8 of circle
			{
				// Draw even octants
				if (mask & 0x01) Draw(x + x0, y - y0, p);// Q6 - upper right right
				if (mask & 0x04) Draw(x + y0, y + x0, p);// Q4 - lower lower right
				if (mask & 0x10) Draw(x - x0, y + y0, p);// Q2 - lower left left
				if (mask & 0x40) Draw(x - y0, y - x0, p);// Q0 - upper upper left
				if (x0 != 0 && x0 != y0)
				{
					if (mask & 0x02) Draw(x + y0, y - x0, p);// Q7 - upper upper right
					if (mask & 0x08) Draw(x + x0, y + y0, p);// Q5 - lower right right
					if (mask & 0x20) Draw(x - y0, y + x0, p);// Q3 - lower lower left
					if (mask & 0x80) Draw(x - x0, y - y0, p);// Q1 - upper left left
				}

				if (d < 0)
					d += 4 * x0++ + 6;
				else
					d += 4 * (x0++ - y0--) + 10;
			}
		}
		else
			Draw(x, y, p);
	}

	void GameEngine::FillCircle(const alo::vi2d& pos, int32_t radius, Pixel p)
	{ FillCircle(pos.x, pos.y, radius, p); }

	void GameEngine::FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
	{ 
		if (radius < 0 || x < -radius || y < -radius || x - GetDrawTargetWidth() > radius || y - GetDrawTargetHeight() > radius)
			return;

		if (radius > 0)
		{
			int x0 = 0;
			int y0 = radius;
			int d = 3 - 2 * radius;

			auto drawline = [&](int sx, int ex, int y)
			{
				for (int x = sx; x <= ex; x++)
					Draw(x, y, p);
			};

			while (y0 >= x0)
			{
				drawline(x - y0, x + y0, y - x0);
				if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

				if (d < 0)
					d += 4 * x0++ + 6;
				else
				{
					if (x0 != y0)
					{
						drawline(x - x0, x + x0, y - y0);
						drawline(x - x0, x + x0, y + y0);
					}
					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}
		else
			Draw(x, y, p);
	}

	void GameEngine::DrawRect(const alo::vi2d& pos, const alo::vi2d& size, Pixel p)
	{ DrawRect(pos.x, pos.y, size.x, size.y, p); }

	void GameEngine::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		DrawLine(x, y, x + w, y, p);
		DrawLine(x + w, y, x + w, y + h, p);
		DrawLine(x + w, y + h, x, y + h, p);
		DrawLine(x, y + h, x, y, p);
	}

	void GameEngine::Clear(Pixel p)
	{
		int pixels = GetDrawTargetWidth() * GetDrawTargetHeight();
		Pixel* m = GetDrawTarget()->GetData();
		for (int i = 0; i < pixels; i++) m[i] = p;
	}

	void GameEngine::ClearBuffer(Pixel p, bool bDepth)
	{ renderer->ClearBuffer(p, bDepth);	}

	alo::Sprite* GameEngine::GetFontSprite()
	{ return fontRenderable.Sprite(); }

	bool GameEngine::ClipLineToScreen(alo::vi2d& in_p1, alo::vi2d& in_p2)
	{
		// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
		static constexpr int SEG_I = 0b0000, SEG_L = 0b0001, SEG_R = 0b0010, SEG_B = 0b0100, SEG_T = 0b1000;
		auto Segment = [&vScreenSize = vScreenSize](const alo::vi2d& v)
		{
			int i = SEG_I;
			if (v.x < 0) i |= SEG_L; else if (v.x > vScreenSize.x) i |= SEG_R;
			if (v.y < 0) i |= SEG_B; else if (v.y > vScreenSize.y) i |= SEG_T;
			return i;
		};

		int s1 = Segment(in_p1), s2 = Segment(in_p2);

		while (true)
		{
			if (!(s1 | s2))	  return true;
			else if (s1 & s2) return false;
			else
			{
				int s3 = s2 > s1 ? s2 : s1;
				alo::vi2d n;
				if (s3 & SEG_T) { n.x = in_p1.x + (in_p2.x - in_p1.x) * (vScreenSize.y - in_p1.y) / (in_p2.y - in_p1.y); n.y = vScreenSize.y; }
				else if (s3 & SEG_B) { n.x = in_p1.x + (in_p2.x - in_p1.x) * (0 - in_p1.y) / (in_p2.y - in_p1.y); n.y = 0; }
				else if (s3 & SEG_R) { n.x = vScreenSize.x; n.y = in_p1.y + (in_p2.y - in_p1.y) * (vScreenSize.x - in_p1.x) / (in_p2.x - in_p1.x); }
				else if (s3 & SEG_L) { n.x = 0; n.y = in_p1.y + (in_p2.y - in_p1.y) * (0 - in_p1.x) / (in_p2.x - in_p1.x); }
				if (s3 == s1) { in_p1 = n; s1 = Segment(in_p1); }
				else { in_p2 = n; s2 = Segment(in_p2); }
			}
		}
		return true;
	}

	void GameEngine::EnablePixelTransfer(const bool bEnable)
	{
		bSuspendTextureTransfer = !bEnable;
	}


	void GameEngine::FillRect(const alo::vi2d& pos, const alo::vi2d& size, Pixel p)
	{ FillRect(pos.x, pos.y, size.x, size.y, p); }

	void GameEngine::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		int32_t x2 = x + w;
		int32_t y2 = y + h;

		if (x < 0) x = 0;
		if (x >= (int32_t)GetDrawTargetWidth()) x = (int32_t)GetDrawTargetWidth();
		if (y < 0) y = 0;
		if (y >= (int32_t)GetDrawTargetHeight()) y = (int32_t)GetDrawTargetHeight();

		if (x2 < 0) x2 = 0;
		if (x2 >= (int32_t)GetDrawTargetWidth()) x2 = (int32_t)GetDrawTargetWidth();
		if (y2 < 0) y2 = 0;
		if (y2 >= (int32_t)GetDrawTargetHeight()) y2 = (int32_t)GetDrawTargetHeight();

		for (int i = x; i < x2; i++)
			for (int j = y; j < y2; j++)
				Draw(i, j, p);
	}

	void GameEngine::DrawTriangle(const alo::vi2d& pos1, const alo::vi2d& pos2, const alo::vi2d& pos3, Pixel p)
	{ DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p); }

	void GameEngine::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		DrawLine(x1, y1, x2, y2, p);
		DrawLine(x2, y2, x3, y3, p);
		DrawLine(x3, y3, x1, y1, p);
	}

	void GameEngine::FillTriangle(const alo::vi2d& pos1, const alo::vi2d& pos2, const alo::vi2d& pos3, Pixel p)
	{ FillTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p); }

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void GameEngine::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
		if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
		if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1);
		if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1);
		if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
		if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;
		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			std::swap(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	void GameEngine::DrawSprite(const alo::vi2d& pos, Sprite* sprite, uint32_t scale, uint8_t flip)
	{ DrawSprite(pos.x, pos.y, sprite, scale, flip); }

	void GameEngine::DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale, uint8_t flip)
	{
		if (sprite == nullptr)
			return;

		int32_t fxs = 0, fxm = 1, fx = 0;
		int32_t fys = 0, fym = 1, fy = 0;
		if (flip & alo::Sprite::Flip::HORIZ) { fxs = sprite->width - 1; fxm = -1; }
		if (flip & alo::Sprite::Flip::VERT) { fys = sprite->height - 1; fym = -1; }

		if (scale > 1)
		{
			fx = fxs;
			for (int32_t i = 0; i < sprite->width; i++, fx += fxm)
			{
				fy = fys;
				for (int32_t j = 0; j < sprite->height; j++, fy += fym)
					for (uint32_t is = 0; is < scale; is++)
						for (uint32_t js = 0; js < scale; js++)
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx, fy));
			}
		}
		else
		{
			fx = fxs;
			for (int32_t i = 0; i < sprite->width; i++, fx += fxm)
			{
				fy = fys;
				for (int32_t j = 0; j < sprite->height; j++, fy += fym)
					Draw(x + i, y + j, sprite->GetPixel(fx, fy));
			}
		}
	}

	void GameEngine::DrawPartialSprite(const alo::vi2d& pos, Sprite* sprite, const alo::vi2d& sourcepos, const alo::vi2d& size, uint32_t scale, uint8_t flip)
	{ DrawPartialSprite(pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale, flip); }

	void GameEngine::DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale, uint8_t flip)
	{
		if (sprite == nullptr)
			return;

		int32_t fxs = 0, fxm = 1, fx = 0;
		int32_t fys = 0, fym = 1, fy = 0;
		if (flip & alo::Sprite::Flip::HORIZ) { fxs = w - 1; fxm = -1; }
		if (flip & alo::Sprite::Flip::VERT) { fys = h - 1; fym = -1; }

		if (scale > 1)
		{
			fx = fxs;
			for (int32_t i = 0; i < w; i++, fx += fxm)
			{
				fy = fys;
				for (int32_t j = 0; j < h; j++, fy += fym)
					for (uint32_t is = 0; is < scale; is++)
						for (uint32_t js = 0; js < scale; js++)
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx + ox, fy + oy));
			}
		}
		else
		{
			fx = fxs;
			for (int32_t i = 0; i < w; i++, fx += fxm)
			{
				fy = fys;
				for (int32_t j = 0; j < h; j++, fy += fym)
					Draw(x + i, y + j, sprite->GetPixel(fx + ox, fy + oy));
			}
		}
	}

	void GameEngine::SetDecalMode(const alo::DecalMode& mode)
	{ nDecalMode = mode; }

	void GameEngine::SetDecalStructure(const alo::DecalStructure& structure)
	{ nDecalStructure = structure; }

	void GameEngine::DrawPartialDecal(const alo::vf2d& pos, alo::Decal* decal, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::vf2d& scale, const alo::Pixel& tint)
	{
		alo::vf2d vScreenSpacePos =
		{
			  (pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			-((pos.y * vInvScreenSize.y) * 2.0f - 1.0f)
		};

		
		alo::vf2d vScreenSpaceDim =
		{
			  ((pos.x + source_size.x * scale.x) * vInvScreenSize.x) * 2.0f - 1.0f,
			-(((pos.y + source_size.y * scale.y) * vInvScreenSize.y) * 2.0f - 1.0f)
		};

		alo::vf2d vWindow = alo::vf2d(vViewSize);
		alo::vf2d vQuantisedPos = ((vScreenSpacePos * vWindow) + alo::vf2d(0.5f, 0.5f)).floor() / vWindow;
		alo::vf2d vQuantisedDim = ((vScreenSpaceDim * vWindow) + alo::vf2d(0.5f, -0.5f)).ceil() / vWindow;

		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vQuantisedPos.x, vQuantisedPos.y }, { vQuantisedPos.x, vQuantisedDim.y }, { vQuantisedDim.x, vQuantisedDim.y }, { vQuantisedDim.x, vQuantisedPos.y } };
		alo::vf2d uvtl = (source_pos + alo::vf2d(0.0001f, 0.0001f)) * decal->vUVScale;
		alo::vf2d uvbr = (source_pos + source_size - alo::vf2d(0.0001f, 0.0001f)) * decal->vUVScale;
		di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };
		di.w = { 1,1,1,1 };
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawPartialDecal(const alo::vf2d& pos, const alo::vf2d& size, alo::Decal* decal, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint)
	{
		alo::vf2d vScreenSpacePos =
		{
			(pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			((pos.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		alo::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * size.x * vInvScreenSize.x),
			vScreenSpacePos.y - (2.0f * size.y * vInvScreenSize.y)
		};

		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		alo::vf2d uvtl = (source_pos) * decal->vUVScale;
		alo::vf2d uvbr = uvtl + ((source_size) * decal->vUVScale);
		di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };
		di.w = { 1,1,1,1 };
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}


	void GameEngine::DrawDecal(const alo::vf2d& pos, alo::Decal* decal, const alo::vf2d& scale, const alo::Pixel& tint)
	{
		alo::vf2d vScreenSpacePos =
		{
			(pos.x * vInvScreenSize.x) * 2.0f - 1.0f,
			((pos.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		alo::vf2d vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(decal->sprite->width) * vInvScreenSize.x)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(decal->sprite->height) * vInvScreenSize.y)) * scale.y
		};

		DecalInstance di;
		di.decal = decal;
		di.points = 4;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		di.uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		di.w = { 1, 1, 1, 1 };
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawExplicitDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::vf2d* uv, const alo::Pixel* col, uint32_t elements)
	{
		DecalInstance di;
		di.decal = decal;
		di.pos.resize(elements);
		di.uv.resize(elements);
		di.w.resize(elements);
		di.tint.resize(elements);
		di.points = elements;
		for (uint32_t i = 0; i < elements; i++)
		{
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = col[i];
			di.w[i] = 1.0f;
		}
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<alo::vf2d>& uv, const alo::Pixel tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.points = uint32_t(pos.size());
		di.pos.resize(di.points);
		di.uv.resize(di.points);
		di.w.resize(di.points);
		di.tint.resize(di.points);
		for (uint32_t i = 0; i < di.points; i++)
		{
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = tint;
			di.w[i] = 1.0f;
		}
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<alo::vf2d>& uv, const std::vector<alo::Pixel> &tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.points = uint32_t(pos.size());
		di.pos.resize(di.points);
		di.uv.resize(di.points);
		di.w.resize(di.points);
		di.tint.resize(di.points);
		for (uint32_t i = 0; i < di.points; i++)
		{
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = tint[i];
			di.w[i] = 1.0f;
		}
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawPolygonDecal(alo::Decal* decal, const std::vector<alo::vf2d>& pos, const std::vector<float>& depth, const std::vector<alo::vf2d>& uv, const alo::Pixel tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.points = uint32_t(pos.size());
		di.pos.resize(di.points);
		di.uv.resize(di.points);
		di.w.resize(di.points);
		di.tint.resize(di.points);
		for (uint32_t i = 0; i < di.points; i++)
		{
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = tint;
			di.w[i] = 1.0f;
		}
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawLineDecal(const alo::vf2d& pos1, const alo::vf2d& pos2, Pixel p)
	{
		auto m = nDecalMode;
		nDecalMode = alo::DecalMode::WIREFRAME;
		DrawPolygonDecal(nullptr, { pos1, pos2 }, { {0, 0}, {0,0} }, p);
		nDecalMode = m;

		/*DecalInstance di;
		di.decal = nullptr;
		di.points = uint32_t(2);
		di.pos.resize(di.points);
		di.uv.resize(di.points);
		di.w.resize(di.points);
		di.tint.resize(di.points);
		di.pos[0] = { (pos1.x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos1.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
		di.uv[0] = { 0.0f, 0.0f };
		di.tint[0] = p;
		di.w[0] = 1.0f;
		di.pos[1] = { (pos2.x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos2.y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
		di.uv[1] = { 0.0f, 0.0f };
		di.tint[1] = p;
		di.w[1] = 1.0f;
		di.mode = olc::DecalMode::WIREFRAME;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);*/
	}

	void GameEngine::DrawRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel col)
	{
		auto m = nDecalMode;
		SetDecalMode(alo::DecalMode::WIREFRAME);
		alo::vf2d vNewSize = size;// (size - olc::vf2d(0.375f, 0.375f)).ceil();
		std::array<alo::vf2d, 4> points = { { {pos}, {pos.x, pos.y + vNewSize.y}, {pos + vNewSize}, {pos.x + vNewSize.x, pos.y} } };
		std::array<alo::vf2d, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<alo::Pixel, 4> cols = { {col, col, col, col} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data(), 4);
		SetDecalMode(m);

	}

	void GameEngine::FillRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel col)
	{
		alo::vf2d vNewSize = size;// (size - olc::vf2d(0.375f, 0.375f)).ceil();
		std::array<alo::vf2d, 4> points = { { {pos}, {pos.x, pos.y + vNewSize.y}, {pos + vNewSize}, {pos.x + vNewSize.x, pos.y} } };
		std::array<alo::vf2d, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<alo::Pixel, 4> cols = { {col, col, col, col} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data(), 4);
	}

	void GameEngine::GradientFillRectDecal(const alo::vf2d& pos, const alo::vf2d& size, const alo::Pixel colTL, const alo::Pixel colBL, const alo::Pixel colBR, const alo::Pixel colTR)
	{
		std::array<alo::vf2d, 4> points = { { {pos}, {pos.x, pos.y + size.y}, {pos + size}, {pos.x + size.x, pos.y} } };
		std::array<alo::vf2d, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<alo::Pixel, 4> cols = { {colTL, colBL, colBR, colTR} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data(), 4);
	}

	void GameEngine::DrawRotatedDecal(const alo::vf2d& pos, alo::Decal* decal, const float fAngle, const alo::vf2d& center, const alo::vf2d& scale, const alo::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.pos.resize(4);
		di.uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		di.w = { 1, 1, 1, 1 };
		di.tint = { tint, tint, tint, tint };
		di.points = 4;
		di.pos[0] = (alo::vf2d(0.0f, 0.0f) - center) * scale;
		di.pos[1] = (alo::vf2d(0.0f, float(decal->sprite->height)) - center) * scale;
		di.pos[2] = (alo::vf2d(float(decal->sprite->width), float(decal->sprite->height)) - center) * scale;
		di.pos[3] = (alo::vf2d(float(decal->sprite->width), 0.0f) - center) * scale;
		float c = cos(fAngle), s = sin(fAngle);
		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = pos + alo::vf2d(di.pos[i].x * c - di.pos[i].y * s, di.pos[i].x * s + di.pos[i].y * c);
			di.pos[i] = di.pos[i] * vInvScreenSize * 2.0f - alo::vf2d(1.0f, 1.0f);
			di.pos[i].y *= -1.0f;
			di.w[i] = 1;
		}
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}


	void GameEngine::DrawPartialRotatedDecal(const alo::vf2d& pos, alo::Decal* decal, const float fAngle, const alo::vf2d& center, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::vf2d& scale, const alo::Pixel& tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.points = 4;
		di.tint = { tint, tint, tint, tint };
		di.w = { 1, 1, 1, 1 };
		di.pos.resize(4);
		di.pos[0] = (alo::vf2d(0.0f, 0.0f) - center) * scale;
		di.pos[1] = (alo::vf2d(0.0f, source_size.y) - center) * scale;
		di.pos[2] = (alo::vf2d(source_size.x, source_size.y) - center) * scale;
		di.pos[3] = (alo::vf2d(source_size.x, 0.0f) - center) * scale;
		float c = cos(fAngle), s = sin(fAngle);
		for (int i = 0; i < 4; i++)
		{
			di.pos[i] = pos + alo::vf2d(di.pos[i].x * c - di.pos[i].y * s, di.pos[i].x * s + di.pos[i].y * c);
			di.pos[i] = di.pos[i] * vInvScreenSize * 2.0f - alo::vf2d(1.0f, 1.0f);
			di.pos[i].y *= -1.0f;
		}

		alo::vf2d uvtl = source_pos * decal->vUVScale;
		alo::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
		di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };
		di.mode = nDecalMode;
		di.structure = nDecalStructure;
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void GameEngine::DrawPartialWarpedDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint)
	{
		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.w = { 1, 1, 1, 1 };
		di.pos.resize(4);
		di.uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		alo::vf2d center;
		float rd = ((pos[2].x - pos[0].x) * (pos[3].y - pos[1].y) - (pos[3].x - pos[1].x) * (pos[2].y - pos[0].y));
		if (rd != 0)
		{
			alo::vf2d uvtl = source_pos * decal->vUVScale;
			alo::vf2d uvbr = uvtl + (source_size * decal->vUVScale);
			di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };

			rd = 1.0f / rd;
			float rn = ((pos[3].x - pos[1].x) * (pos[0].y - pos[1].y) - (pos[3].y - pos[1].y) * (pos[0].x - pos[1].x)) * rd;
			float sn = ((pos[2].x - pos[0].x) * (pos[0].y - pos[1].y) - (pos[2].y - pos[0].y) * (pos[0].x - pos[1].x)) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = pos[0] + rn * (pos[2] - pos[0]);
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = (pos[i] - center).mag();
			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				di.uv[i] *= q; di.w[i] *= q;
				di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			}
			di.mode = nDecalMode;
			di.structure = nDecalStructure;
			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void GameEngine::DrawWarpedDecal(alo::Decal* decal, const alo::vf2d* pos, const alo::Pixel& tint)
	{
		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.w = { 1, 1, 1, 1 };
		di.pos.resize(4);
		di.uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		alo::vf2d center;
		float rd = ((pos[2].x - pos[0].x) * (pos[3].y - pos[1].y) - (pos[3].x - pos[1].x) * (pos[2].y - pos[0].y));
		if (rd != 0)
		{
			rd = 1.0f / rd;
			float rn = ((pos[3].x - pos[1].x) * (pos[0].y - pos[1].y) - (pos[3].y - pos[1].y) * (pos[0].x - pos[1].x)) * rd;
			float sn = ((pos[2].x - pos[0].x) * (pos[0].y - pos[1].y) - (pos[2].y - pos[0].y) * (pos[0].x - pos[1].x)) * rd;
			if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) center = pos[0] + rn * (pos[2] - pos[0]);
			float d[4];	for (int i = 0; i < 4; i++)	d[i] = (pos[i] - center).mag();
			for (int i = 0; i < 4; i++)
			{
				float q = d[i] == 0.0f ? 1.0f : (d[i] + d[(i + 2) & 3]) / d[(i + 2) & 3];
				di.uv[i] *= q; di.w[i] *= q;
				di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			}
			di.mode = nDecalMode;
			di.structure = nDecalStructure;
			vLayers[nTargetLayer].vecDecalInstance.push_back(di);
		}
	}

	void GameEngine::DrawWarpedDecal(alo::Decal* decal, const std::array<alo::vf2d, 4>& pos, const alo::Pixel& tint)
	{ DrawWarpedDecal(decal, pos.data(), tint); }

	void GameEngine::DrawWarpedDecal(alo::Decal* decal, const alo::vf2d(&pos)[4], const alo::Pixel& tint)
	{ DrawWarpedDecal(decal, &pos[0], tint); }

	void GameEngine::DrawPartialWarpedDecal(alo::Decal* decal, const std::array<alo::vf2d, 4>& pos, const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint)
	{ DrawPartialWarpedDecal(decal, pos.data(), source_pos, source_size, tint); }

	void GameEngine::DrawPartialWarpedDecal(alo::Decal* decal, const alo::vf2d(&pos)[4], const alo::vf2d& source_pos, const alo::vf2d& source_size, const alo::Pixel& tint)
	{ DrawPartialWarpedDecal(decal, &pos[0], source_pos, source_size, tint); }

	void GameEngine::DrawStringDecal(const alo::vf2d& pos, const std::string& sText, const Pixel col, const alo::vf2d& scale)
	{
		alo::vf2d spos = { 0.0f, 0.0f };
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = 0; spos.y += 8.0f * scale.y;
			}
			else if (c == '\t')
			{
				spos.x += 8.0f * float(nTabSizeInSpaces) * scale.x;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialDecal(pos + spos, fontRenderable.Decal(), {float(ox) * 8.0f, float(oy) * 8.0f}, {8.0f, 8.0f}, scale, col);
				spos.x += 8.0f * scale.x;
			}
		}
	}

	void GameEngine::DrawStringPropDecal(const alo::vf2d& pos, const std::string& sText, const Pixel col, const alo::vf2d& scale)
	{
		alo::vf2d spos = { 0.0f, 0.0f };
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = 0; spos.y += 8.0f * scale.y;
			}
			else if (c == '\t')
			{
				spos.x += 8.0f * float(nTabSizeInSpaces) * scale.x;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialDecal(pos + spos, fontRenderable.Decal(), { float(ox) * 8.0f + float(vFontSpacing[c - 32].x), float(oy) * 8.0f }, { float(vFontSpacing[c - 32].y), 8.0f }, scale, col);
				spos.x += float(vFontSpacing[c - 32].y) * scale.x;
			}
		}
	}
	
	void GameEngine::DrawRotatedStringDecal(const alo::vf2d& pos, const std::string& sText, const float fAngle, const alo::vf2d& center, const Pixel col, const alo::vf2d& scale)
	{
		alo::vf2d spos = center;
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = center.x; spos.y -= 8.0f;
			}
			else if (c == '\t')
			{
				spos.x += 8.0f * float(nTabSizeInSpaces) * scale.x;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialRotatedDecal(pos, fontRenderable.Decal(), fAngle, spos, { float(ox) * 8.0f, float(oy) * 8.0f }, { 8.0f, 8.0f }, scale, col);
				spos.x -= 8.0f;
			}
		}
	}

	void GameEngine::DrawRotatedStringPropDecal(const alo::vf2d& pos, const std::string& sText, const float fAngle, const alo::vf2d& center, const Pixel col, const alo::vf2d& scale)
	{
		alo::vf2d spos = center;
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = center.x; spos.y -= 8.0f;
			}
			else if (c == '\t')
			{
				spos.x += 8.0f * float(nTabSizeInSpaces) * scale.x;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialRotatedDecal(pos, fontRenderable.Decal(), fAngle, spos, { float(ox) * 8.0f + float(vFontSpacing[c - 32].x), float(oy) * 8.0f }, { float(vFontSpacing[c - 32].y), 8.0f }, scale, col);
				spos.x -= float(vFontSpacing[c - 32].y);
			}
		}
	}

	alo::vi2d GameEngine::GetTextSize(const std::string& s)
	{
		alo::vi2d size = { 0,1 };
		alo::vi2d pos = { 0,1 };
		for (auto c : s)
		{
			if (c == '\n') { pos.y++;  pos.x = 0; }
			else if (c == '\t') { pos.x += nTabSizeInSpaces; }
			else pos.x++;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}
		return size * 8;
	}

	void GameEngine::DrawString(const alo::vi2d& pos, const std::string& sText, Pixel col, uint32_t scale)
	{ DrawString(pos.x, pos.y, sText, col, scale); }

	void GameEngine::DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
	{
		int32_t sx = 0;
		int32_t sy = 0;
		Pixel::Mode m = nPixelMode;
		
		if (m != Pixel::CUSTOM) 
		{
			if (col.a != 255)		SetPixelMode(Pixel::ALPHA);
			else					SetPixelMode(Pixel::MASK);
		}
		for (auto c : sText)
		{
			if (c == '\n')
			{
				sx = 0; sy += 8 * scale;
			}
			else if (c == '\t')
			{
				sx += 8 * nTabSizeInSpaces * scale;
			}
			else			
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;

				if (scale > 1)
				{
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontRenderable.Sprite()->GetPixel(i + ox * 8, j + oy * 8).r > 0)
								for (uint32_t is = 0; is < scale; is++)
									for (uint32_t js = 0; js < scale; js++)
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
				}
				else
				{
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontRenderable.Sprite()->GetPixel(i + ox * 8, j + oy * 8).r > 0)
								Draw(x + sx + i, y + sy + j, col);
				}
				sx += 8 * scale;
			}
		}
		SetPixelMode(m);
	}

	alo::vi2d GameEngine::GetTextSizeProp(const std::string& s)
	{
		alo::vi2d size = { 0,1 };
		alo::vi2d pos = { 0,1 };
		for (auto c : s)
		{
			if (c == '\n') { pos.y += 1;  pos.x = 0; }
			else if (c == '\t') { pos.x += nTabSizeInSpaces * 8; }
			else pos.x += vFontSpacing[c - 32].y;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}

		size.y *= 8;
		return size;
	}

	void GameEngine::DrawStringProp(const alo::vi2d& pos, const std::string& sText, Pixel col, uint32_t scale)
	{ DrawStringProp(pos.x, pos.y, sText, col, scale); }

	void GameEngine::DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
	{
		int32_t sx = 0;
		int32_t sy = 0;
		Pixel::Mode m = nPixelMode;

		if (m != Pixel::CUSTOM)
		{
			if (col.a != 255)		SetPixelMode(Pixel::ALPHA);
			else					SetPixelMode(Pixel::MASK);
		}
		for (auto c : sText)
		{
			if (c == '\n')
			{
				sx = 0; sy += 8 * scale;
			}
			else if (c == '\t')
			{
				sx += 8 * nTabSizeInSpaces * scale;
			}
			else
			{
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;

				if (scale > 1)
				{
					for (int32_t i = 0; i < vFontSpacing[c - 32].y; i++)
						for (int32_t j = 0; j < 8; j++)
							if (fontRenderable.Sprite()->GetPixel(i + ox * 8 + vFontSpacing[c - 32].x, j + oy * 8).r > 0)
								for (int32_t is = 0; is < int(scale); is++)
									for (int32_t js = 0; js < int(scale); js++)
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
				}
				else
				{
					for (int32_t i = 0; i < vFontSpacing[c - 32].y; i++)
						for (int32_t j = 0; j < 8; j++)
							if (fontRenderable.Sprite()->GetPixel(i + ox * 8 + vFontSpacing[c - 32].x, j + oy * 8).r > 0)
								Draw(x + sx + i, y + sy + j, col);
				}
				sx += vFontSpacing[c - 32].y * scale;
			}
		}
		SetPixelMode(m);
	}

	void GameEngine::SetPixelMode(Pixel::Mode m)
	{ nPixelMode = m; }

	Pixel::Mode GameEngine::GetPixelMode()
	{ return nPixelMode; }

	void GameEngine::SetPixelMode(std::function<alo::Pixel(const int x, const int y, const alo::Pixel&, const alo::Pixel&)> pixelMode)
	{
		funcPixelMode = pixelMode;
		nPixelMode = Pixel::Mode::CUSTOM;
	}

	void GameEngine::SetPixelBlend(float fBlend)
	{
		fBlendFactor = fBlend;
		if (fBlendFactor < 0.0f) fBlendFactor = 0.0f;
		if (fBlendFactor > 1.0f) fBlendFactor = 1.0f;
	}

	std::stringstream& GameEngine::ConsoleOut()
	{ return ssConsoleOutput; }

	bool GameEngine::IsConsoleShowing() const
	{ return bConsoleShow; }

	void GameEngine::ConsoleShow(const alo::Key& keyExit, bool bSuspendTime)
	{
		if (bConsoleShow)
			return;

		bConsoleShow = true;		
		bConsoleSuspendTime = bSuspendTime;
		TextEntryEnable(true);
		keyConsoleExit = keyExit;
		pKeyboardState[keyConsoleExit].bHeld = false;
		pKeyboardState[keyConsoleExit].bPressed = false;
		pKeyboardState[keyConsoleExit].bReleased = true;
	}
	
	void GameEngine::ConsoleClear()
	{ sConsoleLines.clear(); }

	void GameEngine::ConsoleCaptureStdOut(const bool bCapture)
	{
		if(bCapture)
			sbufOldCout = std::cout.rdbuf(ssConsoleOutput.rdbuf());
		else
			std::cout.rdbuf(sbufOldCout);
	}

	void GameEngine::UpdateConsole()
	{
		if (GetKey(keyConsoleExit).bPressed)
		{
			TextEntryEnable(false);
			bConsoleSuspendTime = false;
			bConsoleShow = false;
			return;
		}

		// Keep Console sizes based in real screen dimensions
		vConsoleCharacterScale = alo::vf2d(1.0f, 2.0f) / (alo::vf2d(vViewSize) * vInvScreenSize);
		vConsoleSize = (vViewSize / alo::vi2d(8, 16)) - alo::vi2d(2, 4);

		// If console has changed size, simply reset it
		if (vConsoleSize.y != sConsoleLines.size())
		{
			vConsoleCursor = { 0,0 };
			sConsoleLines.clear();
			sConsoleLines.resize(vConsoleSize.y);
		}

		auto TypeCharacter = [&](const char c)
		{
			if (c >= 32 && c < 127)
			{
				sConsoleLines[vConsoleCursor.y].append(1, c);
				vConsoleCursor.x++;
			}

			if( c == '\n' || vConsoleCursor.x >= vConsoleSize.x)
			{
				vConsoleCursor.y++; vConsoleCursor.x = 0;				
			}			

			if (vConsoleCursor.y >= vConsoleSize.y)
			{
				vConsoleCursor.y = vConsoleSize.y - 1;
				for (size_t i = 1; i < vConsoleSize.y; i++)
					sConsoleLines[i - 1] = sConsoleLines[i];
				sConsoleLines[vConsoleCursor.y].clear();
			}
		};

		// Empty out "std::cout", parsing as we go
		while (ssConsoleOutput.rdbuf()->sgetc() != -1)
		{
			char c = ssConsoleOutput.rdbuf()->sbumpc();
			TypeCharacter(c);
		}

		// Draw Shadow
		GradientFillRectDecal({ 0,0 }, alo::vf2d(vScreenSize), alo::PixelF(0, 0, 0.5f, 0.5f), alo::PixelF(0, 0, 0.25f, 0.5f), alo::PixelF(0, 0, 0.25f, 0.5f), alo::PixelF(0, 0, 0.25f, 0.5f));
				
		// Draw the console buffer
		SetDecalMode(alo::DecalMode::NORMAL);
		for (int32_t nLine = 0; nLine < vConsoleSize.y; nLine++)
			DrawStringDecal(alo::vf2d( 1, 1 + float(nLine) ) * vConsoleCharacterScale * 8.0f, sConsoleLines[nLine], alo::WHITE, vConsoleCharacterScale);

		// Draw Input State
		FillRectDecal(alo::vf2d(1 + float((TextEntryGetCursor() + 1)), 1 + float((vConsoleSize.y - 1))) * vConsoleCharacterScale * 8.0f, alo::vf2d(8, 8) * vConsoleCharacterScale, alo::DARK_CYAN);
		DrawStringDecal(alo::vf2d(1, 1 + float((vConsoleSize.y - 1))) * vConsoleCharacterScale * 8.0f, std::string(">") + TextEntryGetString(), alo::YELLOW, vConsoleCharacterScale);		
	}



	void GameEngine::TextEntryEnable(const bool bEnable, const std::string& sText)
	{
		if (bEnable)
		{
			nTextEntryCursor = int32_t(sText.size());
			sTextEntryString = sText;
			bTextEntryEnable = true;
		}
		else
		{
			bTextEntryEnable = false;
		}
	}

	std::string GameEngine::TextEntryGetString() const
	{ return sTextEntryString; }

	int32_t GameEngine::TextEntryGetCursor() const
	{ return nTextEntryCursor; }

	bool GameEngine::IsTextEntryEnabled() const
	{ return bTextEntryEnable; }


	void GameEngine::UpdateTextEntry()
	{
		// Check for typed characters
		for (const auto& key : vKeyboardMap)
			if (GetKey(std::get<0>(key)).bPressed)
			{
				sTextEntryString.insert(nTextEntryCursor, GetKey(alo::Key::SHIFT).bHeld ? std::get<2>(key) : std::get<1>(key));
				nTextEntryCursor++;
			}

		// Check for command characters
		if (GetKey(alo::Key::LEFT).bPressed)
			nTextEntryCursor = std::max(0, nTextEntryCursor - 1);
		if (GetKey(alo::Key::RIGHT).bPressed)
			nTextEntryCursor = std::min(int32_t(sTextEntryString.size()), nTextEntryCursor + 1);
		if (GetKey(alo::Key::BACK).bPressed && nTextEntryCursor > 0)
		{
			sTextEntryString.erase(nTextEntryCursor-1, 1);
			nTextEntryCursor = std::max(0, nTextEntryCursor - 1);
		}
		if (GetKey(alo::Key::DEL).bPressed && nTextEntryCursor < sTextEntryString.size())
			sTextEntryString.erase(nTextEntryCursor, 1);	

		if (GetKey(alo::Key::UP).bPressed)
		{
			if (!sCommandHistory.empty())
			{
				if (sCommandHistoryIt != sCommandHistory.begin())
					sCommandHistoryIt--;

				nTextEntryCursor = int32_t(sCommandHistoryIt->size());
				sTextEntryString = *sCommandHistoryIt;
			}
		}

		if (GetKey(alo::Key::DOWN).bPressed)
		{	
			if (!sCommandHistory.empty())
			{
				if (sCommandHistoryIt != sCommandHistory.end())
				{
					sCommandHistoryIt++;
					if (sCommandHistoryIt != sCommandHistory.end())
					{
						nTextEntryCursor = int32_t(sCommandHistoryIt->size());
						sTextEntryString = *sCommandHistoryIt;
					}
					else
					{
						nTextEntryCursor = 0;
						sTextEntryString = "";
					}
				}
			}
		}

		if (GetKey(alo::Key::ENTER).bPressed)
		{
			if (bConsoleShow)
			{
				std::cout << ">" + sTextEntryString + "\n";
				if (OnConsoleCommand(sTextEntryString))
				{
					sCommandHistory.push_back(sTextEntryString);
					sCommandHistoryIt = sCommandHistory.end();
				}
				sTextEntryString.clear();
				nTextEntryCursor = 0;
			}
			else
			{
				OnTextEntryComplete(sTextEntryString);
				TextEntryEnable(false);
			}
		}
	}

	// User must override these functions as required. I have not made
	// them abstract because I do need a default behaviour to occur if
	// they are not overwritten

	bool GameEngine::OnUserCreate()
	{ return false;	}

	bool GameEngine::OnUserUpdate(float fElapsedTime)
	{ UNUSED(fElapsedTime);  return false; }

	bool GameEngine::OnUserDestroy()
	{ return true; }

	void GameEngine::OnTextEntryComplete(const std::string& sText) { UNUSED(sText); }
	bool GameEngine::OnConsoleCommand(const std::string& sCommand) { UNUSED(sCommand); return false; }
	

	// Externalised API
	void GameEngine::alo_UpdateViewport()
	{
		int32_t ww = vScreenSize.x * vPixelSize.x;
		int32_t wh = vScreenSize.y * vPixelSize.y;
		float wasp = (float)ww / (float)wh;

		if (bPixelCohesion)
		{
			vScreenPixelSize = (vWindowSize / vScreenSize);
			vViewSize = (vWindowSize / vScreenSize) * vScreenSize;
		}
		else
		{
			vViewSize.x = (int32_t)vWindowSize.x;
			vViewSize.y = (int32_t)((float)vViewSize.x / wasp);

			if (vViewSize.y > vWindowSize.y)
			{
				vViewSize.y = vWindowSize.y;
				vViewSize.x = (int32_t)((float)vViewSize.y * wasp);
			}
		}

		vViewPos = (vWindowSize - vViewSize) / 2;
	}

	void GameEngine::alo_UpdateWindowSize(int32_t x, int32_t y)
	{
		vWindowSize = { x, y };
		alo_UpdateViewport();
	}

	void GameEngine::alo_UpdateMouseWheel(int32_t delta)
	{ nMouseWheelDeltaCache += delta; }

	void GameEngine::alo_UpdateMouse(int32_t x, int32_t y)
	{
		// Mouse coords come in screen space
		// But leave in pixel space
		bHasMouseFocus = true;
		vMouseWindowPos = { x, y };
		// Full Screen mode may have a weird viewport we must clamp to
		x -= vViewPos.x;
		y -= vViewPos.y;
		vMousePosCache.x = (int32_t)(((float)x / (float)(vWindowSize.x - (vViewPos.x * 2)) * (float)vScreenSize.x));
		vMousePosCache.y = (int32_t)(((float)y / (float)(vWindowSize.y - (vViewPos.y * 2)) * (float)vScreenSize.y));
		if (vMousePosCache.x >= (int32_t)vScreenSize.x)	vMousePosCache.x = vScreenSize.x - 1;
		if (vMousePosCache.y >= (int32_t)vScreenSize.y)	vMousePosCache.y = vScreenSize.y - 1;
		if (vMousePosCache.x < 0) vMousePosCache.x = 0;
		if (vMousePosCache.y < 0) vMousePosCache.y = 0;
	}

	void GameEngine::alo_UpdateMouseState(int32_t button, bool state)
	{ pMouseNewState[button] = state; }

	void GameEngine::alo_UpdateKeyState(int32_t key, bool state)
	{ pKeyNewState[key] = state; }

	void GameEngine::alo_UpdateMouseFocus(bool state)
	{ bHasMouseFocus = state; }

	void GameEngine::alo_UpdateKeyFocus(bool state)
	{ bHasInputFocus = state; }

	void GameEngine::alo_Reanimate()
	{ bAtomActive = true; }

	bool GameEngine::alo_IsRunning()
	{ return bAtomActive; }

	void GameEngine::alo_Terminate()
	{ bAtomActive = false; }

	void GameEngine::EngineThread()
	{
		// Allow platform to do stuff here if needed, since its now in the
		// context of this thread
		if (platform->ThreadStartUp() == alo::FAIL)	return;

		// Do engine context specific initialization
		alo_PrepareEngine();

		// Create user resources as part of this thread
		for (auto& ext : vExtensions) ext->OnBeforeUserCreate();
		if (!OnUserCreate()) bAtomActive = false;
		for (auto& ext : vExtensions) ext->OnAfterUserCreate();

		while (bAtomActive)
		{
			// Run as fast as possible
			while (bAtomActive) { alo_CoreUpdate(); }

			// Allow the user to free resources if they have overrided the destroy function
			if (!OnUserDestroy())
			{
				// User denied destroy for some reason, so continue running
				bAtomActive = true;
			}
		}

		platform->ThreadCleanUp();
	}

	void GameEngine::alo_PrepareEngine()
	{
		// Start OpenGL, the context is owned by the game thread
		if (platform->CreateGraphics(bFullScreen, bEnableVSYNC, vViewPos, vViewSize) == alo::FAIL) return;

		// Construct default font sheet
		alo_ConstructFontSheet();

		// Create Primary Layer "0"
		CreateLayer();
		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		SetDrawTarget(nullptr);

		m_tp1 = std::chrono::system_clock::now();
		m_tp2 = std::chrono::system_clock::now();
	}


	void GameEngine::alo_CoreUpdate()
	{
		// Handle Timing
		m_tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
		m_tp1 = m_tp2;

		// Our time per frame coefficient
		float fElapsedTime = elapsedTime.count();
		fLastElapsed = fElapsedTime;

		if (bConsoleSuspendTime)
			fElapsedTime = 0.0f;

		// Some platforms will need to check for events
		platform->HandleSystemEvent();

		// Compare hardware input states from previous frame
		auto ScanHardware = [&](HWButton* pKeys, bool* pStateOld, bool* pStateNew, uint32_t nKeyCount)
		{
			for (uint32_t i = 0; i < nKeyCount; i++)
			{
				pKeys[i].bPressed = false;
				pKeys[i].bReleased = false;
				if (pStateNew[i] != pStateOld[i])
				{
					if (pStateNew[i])
					{
						pKeys[i].bPressed = !pKeys[i].bHeld;
						pKeys[i].bHeld = true;
					}
					else
					{
						pKeys[i].bReleased = true;
						pKeys[i].bHeld = false;
					}
				}
				pStateOld[i] = pStateNew[i];
			}
		};

		ScanHardware(pKeyboardState, pKeyOldState, pKeyNewState, 256);
		ScanHardware(pMouseState, pMouseOldState, pMouseNewState, nMouseButtons);

		// Cache mouse coordinates so they remain consistent during frame
		vMousePos = vMousePosCache;
		nMouseWheelDelta = nMouseWheelDeltaCache;
		nMouseWheelDeltaCache = 0;

		if (bTextEntryEnable)
		{
			UpdateTextEntry();
		}

		// Handle Frame Update
		bool bExtensionBlockFrame = false;
		for (auto& ext : vExtensions) bExtensionBlockFrame |= ext->OnBeforeUserUpdate(fElapsedTime);
		if (!bExtensionBlockFrame)
		{
			if (!OnUserUpdate(fElapsedTime)) bAtomActive = false;
		}
		for (auto& ext : vExtensions) ext->OnAfterUserUpdate(fElapsedTime);

		if (bConsoleShow)
		{
			SetDrawTarget((uint8_t)0);
			UpdateConsole();
		}

		// Display Frame
		renderer->UpdateViewport(vViewPos, vViewSize);
		renderer->ClearBuffer(alo::BLACK, true);

		// Layer 0 must always exist
		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		SetDecalMode(DecalMode::NORMAL);
		renderer->PrepareDrawing();

		for (auto layer = vLayers.rbegin(); layer != vLayers.rend(); ++layer)
		{
			if (layer->bShow)
			{
				if (layer->funcHook == nullptr)
				{
					renderer->ApplyTexture(layer->pDrawTarget.Decal()->id);
					if (!bSuspendTextureTransfer && layer->bUpdate)
					{
						layer->pDrawTarget.Decal()->Update();
						layer->bUpdate = false;
					}

					renderer->DrawLayerQuad(layer->vOffset, layer->vScale, layer->tint);

					// Display Decals in order for this layer
					for (auto& decal : layer->vecDecalInstance)
						renderer->DrawDecal(decal);
					layer->vecDecalInstance.clear();
				}
				else
				{
					// Mwa ha ha.... Have Fun!!!
					layer->funcHook();
				}
			}
		}

		

		// Present Graphics to screen
		renderer->DisplayFrame();

		// Update Title Bar
		fFrameTimer += fElapsedTime;
		nFrameCount++;
		if (fFrameTimer >= 1.0f)
		{
			nLastFPS = nFrameCount;
			fFrameTimer -= 1.0f;
			std::string sTitle = "ALO-Coding - Game Engine - " + sAppName + " - FPS: " + std::to_string(nFrameCount);
			platform->SetWindowTitle(sTitle);
			nFrameCount = 0;
		}
	}

	void GameEngine::alo_ConstructFontSheet()
	{
		std::string data;
		data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
		data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
		data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
		data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
		data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
		data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
		data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
		data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
		data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
		data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
		data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
		data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
		data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
		data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
		data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
		data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

		fontRenderable.Create(128, 48);

		int px = 0, py = 0;
		for (size_t b = 0; b < 1024; b += 4)
		{
			uint32_t sym1 = (uint32_t)data[b + 0] - 48;
			uint32_t sym2 = (uint32_t)data[b + 1] - 48;
			uint32_t sym3 = (uint32_t)data[b + 2] - 48;
			uint32_t sym4 = (uint32_t)data[b + 3] - 48;
			uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++)
			{
				int k = r & (1 << i) ? 255 : 0;
				fontRenderable.Sprite()->SetPixel(px, py, alo::Pixel(k, k, k, k));
				if (++py == 48) { px++; py = 0; }
			}
		}

		fontRenderable.Decal()->Update();

		constexpr std::array<uint8_t, 96> vSpacing = { {
			0x03,0x25,0x16,0x08,0x07,0x08,0x08,0x04,0x15,0x15,0x08,0x07,0x15,0x07,0x24,0x08,
			0x08,0x17,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x24,0x15,0x06,0x07,0x16,0x17,
			0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x17,0x08,0x08,0x08,
			0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x08,0x08,0x17,0x08,0x15,0x08,0x15,0x08,0x08,
			0x24,0x18,0x17,0x17,0x17,0x17,0x17,0x17,0x17,0x33,0x17,0x17,0x33,0x18,0x17,0x17,
			0x17,0x17,0x17,0x17,0x07,0x17,0x17,0x18,0x18,0x17,0x17,0x07,0x33,0x07,0x08,0x00, } };

		for (auto c : vSpacing) vFontSpacing.push_back({ c >> 4, c & 15 });

		// UK Standard Layout
#ifdef ALO_KEYBOARD_UK
		vKeyboardMap =
		{
			{alo::Key::A, "a", "A"}, {alo::Key::B, "b", "B"}, {alo::Key::C, "c", "C"}, {alo::Key::D, "d", "D"}, {alo::Key::E, "e", "E"},
			{alo::Key::F, "f", "F"}, {alo::Key::G, "g", "G"}, {alo::Key::H, "h", "H"}, {alo::Key::I, "i", "I"}, {alo::Key::J, "j", "J"},
			{alo::Key::K, "k", "K"}, {alo::Key::L, "l", "L"}, {alo::Key::M, "m", "M"}, {alo::Key::N, "n", "N"}, {alo::Key::O, "o", "O"},
			{alo::Key::P, "p", "P"}, {alo::Key::Q, "q", "Q"}, {alo::Key::R, "r", "R"}, {alo::Key::S, "s", "S"}, {alo::Key::T, "t", "T"},
			{alo::Key::U, "u", "U"}, {alo::Key::V, "v", "V"}, {alo::Key::W, "w", "W"}, {alo::Key::X, "x", "X"}, {alo::Key::Y, "y", "Y"},
			{alo::Key::Z, "z", "Z"},

			{alo::Key::K0, "0", ")"}, {alo::Key::K1, "1", "!"}, {alo::Key::K2, "2", "\""}, {alo::Key::K3, "3", "#"},	{alo::Key::K4, "4", "$"},
			{alo::Key::K5, "5", "%"}, {alo::Key::K6, "6", "^"}, {alo::Key::K7, "7", "&"}, {alo::Key::K8, "8", "*"},	{alo::Key::K9, "9", "("},

			{alo::Key::NP0, "0", "0"}, {alo::Key::NP1, "1", "1"}, {alo::Key::NP2, "2", "2"}, {alo::Key::NP3, "3", "3"},	{alo::Key::NP4, "4", "4"},
			{alo::Key::NP5, "5", "5"}, {alo::Key::NP6, "6", "6"}, {alo::Key::NP7, "7", "7"}, {alo::Key::NP8, "8", "8"},	{alo::Key::NP9, "9", "9"},
			{alo::Key::NP_MUL, "*", "*"}, {alo::Key::NP_DIV, "/", "/"}, {alo::Key::NP_ADD, "+", "+"}, {alo::Key::NP_SUB, "-", "-"},	{alo::Key::NP_DECIMAL, ".", "."},

			{alo::Key::PERIOD, ".", ">"}, {alo::Key::EQUALS, "=", "+"}, {alo::Key::COMMA, ",", "<"}, {alo::Key::MINUS, "-", "_"}, {alo::Key::SPACE, " ", " "},

			{alo::Key::OEM_1, ";", ":"}, {alo::Key::OEM_2, "/", "?"}, {alo::Key::OEM_3, "\'", "@"}, {alo::Key::OEM_4, "[", "{"},
			{alo::Key::OEM_5, "\\", "|"}, {alo::Key::OEM_6, "]", "}"}, {alo::Key::OEM_7, "#", "~"}, 
			
			// {alo::Key::TAB, "\t", "\t"}
		};
#endif
	}

	void GameEngine::gex_Register(alo::GEX* gex)
	{
		if (std::find(vExtensions.begin(), vExtensions.end(), gex) == vExtensions.end())
			vExtensions.push_back(gex);			
	}


	GEX::GEX(bool bHook) { if(bHook) ge->gex_Register(this); }
	void GEX::OnBeforeUserCreate() {}
	void GEX::OnAfterUserCreate()	{}
	bool GEX::OnBeforeUserUpdate(float& fElapsedTime) { return false; }
	void GEX::OnAfterUserUpdate(float fElapsedTime) {}

	// Need a couple of statics as these are singleton instances
	// read from multiple locations
	std::atomic<bool> GameEngine::bAtomActive{ false };
	alo::GameEngine* alo::GEX::ge = nullptr;
	alo::GameEngine* alo::Platform::ptrGE = nullptr;
	alo::GameEngine* alo::Renderer::ptrGE = nullptr;
	std::unique_ptr<ImageLoader> alo::Sprite::loader = nullptr;
};
#pragma endregion 

// O------------------------------------------------------------------------------O
// | aloGameEngine Renderers - the draw-y bits                                    |
// O------------------------------------------------------------------------------O

#if !defined(ALO_GE_HEADLESS)

#pragma region renderer_ogl10
// O------------------------------------------------------------------------------O
// | START RENDERER: OpenGL 1.0 (the original, the best...)                       |
// O------------------------------------------------------------------------------O
#if defined(ALO_GFX_OPENGL10)

#if defined(ALO_PLATFORM_WINAPI)
	#include <dwmapi.h>
	#include <GL/gl.h>
	#if !defined(__MINGW32__)
		#pragma comment(lib, "Dwmapi.lib")
	#endif
	typedef BOOL(WINAPI wglSwapInterval_t) (int interval);
	static wglSwapInterval_t* wglSwapInterval = nullptr;
	typedef HDC glDeviceContext_t;
	typedef HGLRC glRenderContext_t;
#endif

#if defined(ALO_PLATFORM_X11)
	namespace X11
	{
		#include <GL/glx.h>
	}
	typedef int(glSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
	static glSwapInterval_t* glSwapIntervalEXT;
	typedef X11::GLXContext glDeviceContext_t;
	typedef X11::GLXContext glRenderContext_t;
#endif

namespace alo
{
	class Renderer_OGL10 : public alo::Renderer
	{
	private:
#if defined(ALO_PLATFORM_GLUT)
		bool mFullScreen = false;
#else
		glDeviceContext_t glDeviceContext = 0;
		glRenderContext_t glRenderContext = 0;
#endif

		bool bSync = false;
		alo::DecalMode nDecalMode = alo::DecalMode(-1);
		alo::DecalStructure nDecalStructure = alo::DecalStructure(-1);
#if defined(ALO_PLATFORM_X11)
		X11::Display* alo_Display = nullptr;
		X11::Window* alo_Window = nullptr;
		X11::XVisualInfo* alo_VisualInfo = nullptr;
#endif

	public:
		void PrepareDevice() override
		{
#if defined(ALO_PLATFORM_GLUT)
			//glutInit has to be called with main() arguments, make fake ones
			int argc = 0;
			char* argv[1] = { (char*)"" };
			glutInit(&argc, argv);
			glutInitWindowPosition(0, 0);
			glutInitWindowSize(512, 512);
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
			// Creates the window and the OpenGL context for it
			glutCreateWindow("ALO-Coding - Game Engine");
			glEnable(GL_TEXTURE_2D); // Turn on texturing
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
		}

		alo::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) override
		{
#if defined(ALO_PLATFORM_WINAPI)
			// Create Device Context
			glDeviceContext = GetDC((HWND)(params[0]));
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR), 1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				PFD_MAIN_PLANE, 0, 0, 0, 0
			};

			int pf = 0;
			if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd))) return alo::FAIL;
			SetPixelFormat(glDeviceContext, pf, &pfd);

			if (!(glRenderContext = wglCreateContext(glDeviceContext))) return alo::FAIL;
			wglMakeCurrent(glDeviceContext, glRenderContext);

			// Remove Frame cap
			wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
			if (wglSwapInterval && !bVSYNC) wglSwapInterval(0);
			bSync = bVSYNC;
#endif

#if defined(ALO_PLATFORM_X11)
			using namespace X11;
			// Linux has tighter coupling between OpenGL and X11, so we store
			// various "platform" handles in the renderer
			alo_Display = (X11::Display*)(params[0]);
			alo_Window = (X11::Window*)(params[1]);
			alo_VisualInfo = (X11::XVisualInfo*)(params[2]);

			glDeviceContext = glXCreateContext(alo_Display, alo_VisualInfo, nullptr, GL_TRUE);
			glXMakeCurrent(alo_Display, *alo_Window, glDeviceContext);

			XWindowAttributes gwa;
			XGetWindowAttributes(alo_Display, *alo_Window, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);

			glSwapIntervalEXT = nullptr;
			glSwapIntervalEXT = (glSwapInterval_t*)glXGetProcAddress((unsigned char*)"glXSwapIntervalEXT");

			if (glSwapIntervalEXT == nullptr && !bVSYNC)
			{
				printf("NOTE: Could not disable VSYNC, glXSwapIntervalEXT() was not found!\n");
				printf("      Don't worry though, things will still work, it's just the\n");
				printf("      frame rate will be capped to your monitors refresh rate\n");
			}

			if (glSwapIntervalEXT != nullptr && !bVSYNC)
				glSwapIntervalEXT(alo_Display, *alo_Window, 0);
#endif		

#if defined(ALO_PLATFORM_GLUT)
			mFullScreen = bFullScreen;
			if (!bVSYNC)
			{
#if defined(__APPLE__)
				GLint sync = 0;
				CGLContextObj ctx = CGLGetCurrentContext();
				if (ctx) CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
#endif
			}
#else
			glEnable(GL_TEXTURE_2D); // Turn on texturing
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
			return alo::rcode::OK;
		}

		alo::rcode DestroyDevice() override
		{
#if defined(ALO_PLATFORM_WINAPI)
			wglDeleteContext(glRenderContext);
#endif

#if defined(ALO_PLATFORM_X11)
			glXMakeCurrent(alo_Display, None, NULL);
			glXDestroyContext(alo_Display, glDeviceContext);
#endif

#if defined(ALO_PLATFORM_GLUT)
			glutDestroyWindow(glutGetWindow());
#endif
			return alo::rcode::OK;
		}

		void DisplayFrame() override
		{
#if defined(ALO_PLATFORM_WINAPI)
			SwapBuffers(glDeviceContext);
			if (bSync) DwmFlush();
#endif	

#if defined(ALO_PLATFORM_X11)
			X11::glXSwapBuffers(alo_Display, *alo_Window);
#endif		

#if defined(ALO_PLATFORM_GLUT)
			glutSwapBuffers();
#endif
		}

		void PrepareDrawing() override
		{
			
			//ClearBuffer(olc::GREEN, true);
			glEnable(GL_BLEND);
			nDecalMode = DecalMode::NORMAL;
			nDecalStructure = DecalStructure::FAN;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		void SetDecalMode(const alo::DecalMode& mode)
		{
			if (mode != nDecalMode)
			{
				switch (mode)
				{
				case alo::DecalMode::NORMAL:
				case alo::DecalMode::MODEL3D:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case alo::DecalMode::ADDITIVE:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					break;
				case alo::DecalMode::MULTIPLICATIVE:
					glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case alo::DecalMode::STENCIL:
					glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
					break;
				case alo::DecalMode::ILLUMINATE:
					glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
					break;
				case alo::DecalMode::WIREFRAME:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				}

				nDecalMode = mode;
			}
		}

		void DrawLayerQuad(const alo::vf2d& offset, const alo::vf2d& scale, const alo::Pixel tint) override
		{
			glBegin(GL_QUADS);
			glColor4ub(tint.r, tint.g, tint.b, tint.a);
			glTexCoord2f(0.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
			glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
			glTexCoord2f(0.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
			glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
			glTexCoord2f(1.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
			glVertex3f(1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
			glTexCoord2f(1.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
			glVertex3f(1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
			glEnd();
		}

		void DrawDecal(const alo::DecalInstance& decal) override
		{
			SetDecalMode(decal.mode);

			if (decal.decal == nullptr)
				glBindTexture(GL_TEXTURE_2D, 0);
			else
				glBindTexture(GL_TEXTURE_2D, decal.decal->id);
			
			if (nDecalMode == DecalMode::MODEL3D)
			{
#ifdef ALO_ENABLE_EXPERIMENTAL
				glMatrixMode(GL_PROJECTION); glPushMatrix();
				glMatrixMode(GL_MODELVIEW);  glPushMatrix();

				glEnable(GL_DEPTH_TEST);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1, 1000);

				#pragma comment (lib, "winmm.lib")

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glTranslatef(0, -40, -200);
				glRotatef(float(clock()) * 0.1f, 1, 0, 0);
				glRotatef(float(clock()) * 0.1f * 2, 0, 1, 0);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glBegin(GL_TRIANGLES);
				

				// Render as 3D Spatial Entity
				for (uint32_t n = 0; n < decal.points; n++)
				{
					glColor4ub(decal.tint[n].r, decal.tint[n].g, decal.tint[n].b, decal.tint[n].a);
					glTexCoord2f(decal.uv[n].x, decal.uv[n].y);
					glVertex3f(decal.pos[n].x, decal.pos[n].y, decal.w[n]);
				}

				glEnd();

				glMatrixMode(GL_PROJECTION); glPopMatrix();
				glMatrixMode(GL_MODELVIEW);  glPopMatrix();
				glDisable(GL_DEPTH_TEST);
#endif
			}
			else
			{
				if (nDecalMode == DecalMode::WIREFRAME)
					glBegin(GL_LINE_LOOP);
				else
				{
					if(decal.structure == alo::DecalStructure::FAN)
						glBegin(GL_TRIANGLE_FAN);
					else if(decal.structure == alo::DecalStructure::STRIP)
						glBegin(GL_TRIANGLE_STRIP);
					else if(decal.structure == alo::DecalStructure::LIST)
						glBegin(GL_TRIANGLES);
				}

				// Render as 2D Spatial entity
				for (uint32_t n = 0; n < decal.points; n++)
				{
					glColor4ub(decal.tint[n].r, decal.tint[n].g, decal.tint[n].b, decal.tint[n].a);
					glTexCoord4f(decal.uv[n].x, decal.uv[n].y, 0.0f, decal.w[n]);
					glVertex2f(decal.pos[n].x, decal.pos[n].y);
				}

				glEnd();
			}
			

			//glDisable(GL_DEPTH_TEST);
		}

		uint32_t CreateTexture(const uint32_t width, const uint32_t height, const bool filtered, const bool clamp) override
		{
			UNUSED(width);
			UNUSED(height);
			uint32_t id = 0;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			if (filtered)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			if (clamp)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			return id;
		}

		uint32_t DeleteTexture(const uint32_t id) override
		{
			glDeleteTextures(1, &id);
			return id;
		}

		void UpdateTexture(uint32_t id, alo::Sprite* spr) override
		{
			UNUSED(id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->width, spr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
		}

		void ReadTexture(uint32_t id, alo::Sprite* spr) override
		{
			glReadPixels(0, 0, spr->width, spr->height, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
		}

		void ApplyTexture(uint32_t id) override
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}

		void ClearBuffer(alo::Pixel p, bool bDepth) override
		{
			glClearColor(float(p.r) / 255.0f, float(p.g) / 255.0f, float(p.b) / 255.0f, float(p.a) / 255.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (bDepth) glClear(GL_DEPTH_BUFFER_BIT);
		}

		void UpdateViewport(const alo::vi2d& pos, const alo::vi2d& size) override
		{
			glViewport(pos.x, pos.y, size.x, size.y);
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | END RENDERER: OpenGL 1.0 (the original, the best...)                         |
// O------------------------------------------------------------------------------O
#pragma endregion

#pragma region renderer_ogl33
// O------------------------------------------------------------------------------O
// | START RENDERER: OpenGL 3.3 (3.0 es) (sh-sh-sh-shaders....)                   |
// O------------------------------------------------------------------------------O
#if defined(ALO_GFX_OPENGL33)

#if defined(ALO_PLATFORM_WINAPI)
	#include <dwmapi.h>
	#include <gl/GL.h>
	#if !defined(__MINGW32__)
		#pragma comment(lib, "Dwmapi.lib")
	#endif		
	typedef void __stdcall locSwapInterval_t(GLsizei n);
	typedef HDC glDeviceContext_t;
	typedef HGLRC glRenderContext_t;
	#define CALLSTYLE __stdcall
	#define OGL_LOAD(t, n) (t*)wglGetProcAddress(#n)
#endif

#if defined(ALO_PLATFORM_X11)
	namespace X11
	{
		#include <GL/glx.h>
	}
	typedef int(locSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
	typedef X11::GLXContext glDeviceContext_t;
	typedef X11::GLXContext glRenderContext_t;
	#define CALLSTYLE 
	#define OGL_LOAD(t, n) (t*)glXGetProcAddress((unsigned char*)#n);
#endif

namespace alo
{
	typedef char GLchar;
	typedef ptrdiff_t GLsizeiptr;
	typedef GLuint CALLSTYLE locCreateShader_t(GLenum type);
	typedef GLuint CALLSTYLE locCreateProgram_t(void);
	typedef void CALLSTYLE locDeleteShader_t(GLuint shader);
#if defined(ALO_PLATFORM_EMSCRIPTEN)
	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
#else
	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
#endif
	typedef void CALLSTYLE locCompileShader_t(GLuint shader);
	typedef void CALLSTYLE locLinkProgram_t(GLuint program);
	typedef void CALLSTYLE locDeleteProgram_t(GLuint program);
	typedef void CALLSTYLE locAttachShader_t(GLuint program, GLuint shader);
	typedef void CALLSTYLE locBindBuffer_t(GLenum target, GLuint buffer);
	typedef void CALLSTYLE locBufferData_t(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
	typedef void CALLSTYLE locGenBuffers_t(GLsizei n, GLuint* buffers);
	typedef void CALLSTYLE locVertexAttribPointer_t(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
	typedef void CALLSTYLE locEnableVertexAttribArray_t(GLuint index);
	typedef void CALLSTYLE locUseProgram_t(GLuint program);
	typedef void CALLSTYLE locBindVertexArray_t(GLuint array);
	typedef void CALLSTYLE locGenVertexArrays_t(GLsizei n, GLuint* arrays);
	typedef void CALLSTYLE locGetShaderInfoLog_t(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

	constexpr size_t ALO_MAX_VERTS = 128;

	class Renderer_OGL33 : public alo::Renderer
	{
	private:

#if defined(ALO_PLATFORM_GLUT)
		bool mFullScreen = false;
#endif
		bool bSync = false;
		alo::DecalMode nDecalMode = alo::DecalMode(-1); // Thanks Gusgo & Bispoo
#if defined(ALO_PLATFORM_X11)
		X11::Display* alo_Display = nullptr;
		X11::Window* alo_Window = nullptr;
		X11::XVisualInfo* alo_VisualInfo = nullptr;
#endif

	private:
		locCreateShader_t* locCreateShader = nullptr;
		locShaderSource_t* locShaderSource = nullptr;
		locCompileShader_t* locCompileShader = nullptr;
		locDeleteShader_t* locDeleteShader = nullptr;
		locCreateProgram_t* locCreateProgram = nullptr;
		locDeleteProgram_t* locDeleteProgram = nullptr;
		locLinkProgram_t* locLinkProgram = nullptr;
		locAttachShader_t* locAttachShader = nullptr;
		locBindBuffer_t* locBindBuffer = nullptr;
		locBufferData_t* locBufferData = nullptr;
		locGenBuffers_t* locGenBuffers = nullptr;
		locVertexAttribPointer_t* locVertexAttribPointer = nullptr;
		locEnableVertexAttribArray_t* locEnableVertexAttribArray = nullptr;
		locUseProgram_t* locUseProgram = nullptr;
		locBindVertexArray_t* locBindVertexArray = nullptr;
		locGenVertexArrays_t* locGenVertexArrays = nullptr;
		locSwapInterval_t* locSwapInterval = nullptr;
		locGetShaderInfoLog_t* locGetShaderInfoLog = nullptr;

		uint32_t m_nFS = 0;
		uint32_t m_nVS = 0;
		uint32_t m_nQuadShader = 0;
		uint32_t m_vbQuad = 0;
		uint32_t m_vaQuad = 0;

		struct locVertex
		{
			float pos[3];
			alo::vf2d tex;
			alo::Pixel col;
		};

		locVertex pVertexMem[OLC_MAX_VERTS];

		alo::Renderable rendBlankQuad;

	public:
		void PrepareDevice() override
		{
#if defined(ALO_PLATFORM_GLUT)
			//glutInit has to be called with main() arguments, make fake ones
			int argc = 0;
			char* argv[1] = { (char*)"" };
			glutInit(&argc, argv);
			glutInitWindowPosition(0, 0);
			glutInitWindowSize(512, 512);
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
			// Creates the window and the OpenGL context for it
			glutCreateWindow("ALO-Coding Game Engine");
			glEnable(GL_TEXTURE_2D); // Turn on texturing
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
		}

		alo::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) override
		{
			// Create OpenGL Context
#if defined(ALO_PLATFORM_WINAPI)
			// Create Device Context
			glDeviceContext = GetDC((HWND)(params[0]));
			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR), 1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				PFD_MAIN_PLANE, 0, 0, 0, 0
			};

			int pf = 0;
			if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd))) return alo::FAIL;
			SetPixelFormat(glDeviceContext, pf, &pfd);

			if (!(glRenderContext = wglCreateContext(glDeviceContext))) return alo::FAIL;
			wglMakeCurrent(glDeviceContext, glRenderContext);

			// Set Vertical Sync
			locSwapInterval = OGL_LOAD(locSwapInterval_t, "wglSwapIntervalEXT");
			if (locSwapInterval && !bVSYNC) locSwapInterval(0);
			bSync = bVSYNC;
#endif

#if defined(ALO_PLATFORM_X11)
			using namespace X11;
			// Linux has tighter coupling between OpenGL and X11, so we store
			// various "platform" handles in the renderer
			alo_Display = (X11::Display*)(params[0]);
			alo_Window = (X11::Window*)(params[1]);
			alo_VisualInfo = (X11::XVisualInfo*)(params[2]);

			glDeviceContext = glXCreateContext(alo_Display, alo_VisualInfo, nullptr, GL_TRUE);
			glXMakeCurrent(alo_Display, *alo_Window, glDeviceContext);

			XWindowAttributes gwa;
			XGetWindowAttributes(alo_Display, *alo_Window, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);

			locSwapInterval = OGL_LOAD(locSwapInterval_t, "glXSwapIntervalEXT");

			if (locSwapInterval == nullptr && !bVSYNC)
			{
				printf("NOTE: Could not disable VSYNC, glXSwapIntervalEXT() was not found!\n");
				printf("      Don't worry though, things will still work, it's just the\n");
				printf("      frame rate will be capped to your monitors refresh rate - javidx9\n");
			}

			if (locSwapInterval != nullptr && !bVSYNC)
				locSwapInterval(alo_Display, *alo_Window, 0);
#endif		

#if defined(ALO_PLATFORM_EMSCRIPTEN)
			EGLint const attribute_list[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE };
			EGLint const context_config[] = { EGL_CONTEXT_CLIENT_VERSION , 2, EGL_NONE };
			EGLint num_config;

			alo_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			eglInitialize(alo_Display, nullptr, nullptr);
			eglChooseConfig(alo_Display, attribute_list, &alo_Config, 1, &num_config);

			/* create an EGL rendering context */
			alo_Context = eglCreateContext(alo_Display, alo_Config, EGL_NO_CONTEXT, context_config);
			alo_Surface = eglCreateWindowSurface(alo_Display, alo_Config, NULL, nullptr);
			eglMakeCurrent(alo_Display, alo_Surface, alo_Surface, alo_Context);
			//eglSwapInterval is currently a NOP, plement anyways in case it becomes supported
			locSwapInterval = &eglSwapInterval;
			locSwapInterval(alo_Display, bVSYNC ? 1 : 0);
#endif

#if defined(ALO_PLATFORM_GLUT)
			mFullScreen = bFullScreen;
			if (!bVSYNC)
			{
			}
#else
	
#endif
			// Load External OpenGL Functions
			locCreateShader = OGL_LOAD(locCreateShader_t, glCreateShader);
			locCompileShader = OGL_LOAD(locCompileShader_t, glCompileShader);
			locShaderSource = OGL_LOAD(locShaderSource_t, glShaderSource);
			locDeleteShader = OGL_LOAD(locDeleteShader_t, glDeleteShader);
			locCreateProgram = OGL_LOAD(locCreateProgram_t, glCreateProgram);
			locDeleteProgram = OGL_LOAD(locDeleteProgram_t, glDeleteProgram);
			locLinkProgram = OGL_LOAD(locLinkProgram_t, glLinkProgram);
			locAttachShader = OGL_LOAD(locAttachShader_t, glAttachShader);
			locBindBuffer = OGL_LOAD(locBindBuffer_t, glBindBuffer);
			locBufferData = OGL_LOAD(locBufferData_t, glBufferData);
			locGenBuffers = OGL_LOAD(locGenBuffers_t, glGenBuffers);
			locVertexAttribPointer = OGL_LOAD(locVertexAttribPointer_t, glVertexAttribPointer);
			locEnableVertexAttribArray = OGL_LOAD(locEnableVertexAttribArray_t, glEnableVertexAttribArray);
			locUseProgram = OGL_LOAD(locUseProgram_t, glUseProgram);
			locGetShaderInfoLog = OGL_LOAD(locGetShaderInfoLog_t, glGetShaderInfoLog);

			// Load & Compile Quad Shader - assumes no errors
			m_nFS = locCreateShader(0x8B30);
			const GLchar* strFS =

				"out vec4 pixel;\n""in vec2 oTex;\n"
				"in vec4 oCol;\n""uniform sampler2D sprTex;\n""void main(){pixel = texture(sprTex, oTex) * oCol;}";
			locShaderSource(m_nFS, 1, &strFS, NULL);
			locCompileShader(m_nFS);

			m_nVS = locCreateShader(0x8B31);
			const GLchar* strVS =

				"layout(location = 0) in vec3 aPos;\n""layout(location = 1) in vec2 aTex;\n"
				"layout(location = 2) in vec4 aCol;\n""out vec2 oTex;\n""out vec4 oCol;\n"
				"void main(){ float p = 1.0 / aPos.z; gl_Position = p * vec4(aPos.x, aPos.y, 0.0, 1.0); oTex = p * aTex; oCol = aCol;}";
			locShaderSource(m_nVS, 1, &strVS, NULL);
			locCompileShader(m_nVS);

			m_nQuadShader = locCreateProgram();
			locAttachShader(m_nQuadShader, m_nFS);
			locAttachShader(m_nQuadShader, m_nVS);
			locLinkProgram(m_nQuadShader);

			// Create Quad
			locGenBuffers(1, &m_vbQuad);
			locGenVertexArrays(1, &m_vaQuad);
			locBindVertexArray(m_vaQuad);
			locBindBuffer(0x8892, m_vbQuad);

			locVertex verts[OLC_MAX_VERTS];
			locBufferData(0x8892, sizeof(locVertex) * OLC_MAX_VERTS, verts, 0x88E0);
			locVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(locVertex), 0); locEnableVertexAttribArray(0);
			locVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(locVertex), (void*)(3 * sizeof(float))); locEnableVertexAttribArray(1);
			locVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(locVertex), (void*)(5 * sizeof(float)));	locEnableVertexAttribArray(2);
			locBindBuffer(0x8892, 0);
			locBindVertexArray(0);

			// Create blank texture for spriteless decals
			rendBlankQuad.Create(1, 1);
			rendBlankQuad.Sprite()->GetData()[0] = alo::WHITE;
			rendBlankQuad.Decal()->Update();
			return alo::rcode::OK;
		}

		alo::rcode DestroyDevice() override
		{
#if defined(ALO_PLATFORM_WINAPI)
			wglDeleteContext(glRenderContext);
#endif

#if defined(ALO_PLATFORM_X11)
			glXMakeCurrent(alo_Display, None, NULL);
			glXDestroyContext(alo_Display, glDeviceContext);
#endif

#if defined(ALO_PLATFORM_GLUT)
			glutDestroyWindow(glutGetWindow());
#endif

			return alo::rcode::OK;
		}

		void DisplayFrame() override
		{
#if defined(ALO_PLATFORM_WINAPI)
			SwapBuffers(glDeviceContext);
			if (bSync) DwmFlush();
#endif	

#if defined(ALO_PLATFORM_X11)
			X11::glXSwapBuffers(alo_Display, *alo_Window);
#endif		

#if defined(ALO_PLATFORM_GLUT)
			glutSwapBuffers();
#endif

		}

		void PrepareDrawing() override
		{
			glEnable(GL_BLEND);
			nDecalMode = DecalMode::NORMAL;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			locUseProgram(m_nQuadShader);
			locBindVertexArray(m_vaQuad);
		}

		void SetDecalMode(const alo::DecalMode& mode) override
		{
			if (mode != nDecalMode)
			{
				switch (mode)
				{
				case alo::DecalMode::NORMAL: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	break;
				case alo::DecalMode::ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
				case alo::DecalMode::MULTIPLICATIVE: glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);	break;
				case alo::DecalMode::STENCIL: glBlendFunc(GL_ZERO, GL_SRC_ALPHA); break;
				case alo::DecalMode::ILLUMINATE: glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);	break;
				case alo::DecalMode::WIREFRAME: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	break;
				}

				nDecalMode = mode;
			}
		}

		void DrawLayerQuad(const alo::vf2d& offset, const alo::vf2d& scale, const alo::Pixel tint) override
		{
			locBindBuffer(0x8892, m_vbQuad);
			locVertex verts[4] = {
				{{-1.0f, -1.0f, 1.0}, {0.0f * scale.x + offset.x, 1.0f * scale.y + offset.y}, tint},
				{{+1.0f, -1.0f, 1.0}, {1.0f * scale.x + offset.x, 1.0f * scale.y + offset.y}, tint},
				{{-1.0f, +1.0f, 1.0}, {0.0f * scale.x + offset.x, 0.0f * scale.y + offset.y}, tint},
				{{+1.0f, +1.0f, 1.0}, {1.0f * scale.x + offset.x, 0.0f * scale.y + offset.y}, tint},
			};

			locBufferData(0x8892, sizeof(locVertex) * 4, verts, 0x88E0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		void DrawDecal(const alo::DecalInstance& decal) override
		{
			SetDecalMode(decal.mode);
			if (decal.decal == nullptr)
				glBindTexture(GL_TEXTURE_2D, rendBlankQuad.Decal()->id);
			else
				glBindTexture(GL_TEXTURE_2D, decal.decal->id);

			locBindBuffer(0x8892, m_vbQuad);

			for (uint32_t i = 0; i < decal.points; i++)
				pVertexMem[i] = { { decal.pos[i].x, decal.pos[i].y, decal.w[i] }, { decal.uv[i].x, decal.uv[i].y }, decal.tint[i] };

			locBufferData(0x8892, sizeof(locVertex) * decal.points, pVertexMem, 0x88E0);

			if (nDecalMode == DecalMode::WIREFRAME)
				glDrawArrays(GL_LINE_LOOP, 0, decal.points);
			else
				glDrawArrays(GL_TRIANGLE_FAN, 0, decal.points);
		}

		uint32_t CreateTexture(const uint32_t width, const uint32_t height, const bool filtered, const bool clamp) override
		{
			UNUSED(width);
			UNUSED(height);
			uint32_t id = 0;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			if (filtered)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			if (clamp)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			return id;
		}

		uint32_t DeleteTexture(const uint32_t id) override
		{
			glDeleteTextures(1, &id);
			return id;
		}

		void UpdateTexture(uint32_t id, alo::Sprite* spr) override
		{
			UNUSED(id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->width, spr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
		}

		void ReadTexture(uint32_t id, alo::Sprite* spr) override
		{
			glReadPixels(0, 0, spr->width, spr->height, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
		}

		void ApplyTexture(uint32_t id) override
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}

		void ClearBuffer(alo::Pixel p, bool bDepth) override
		{
			glClearColor(float(p.r) / 255.0f, float(p.g) / 255.0f, float(p.b) / 255.0f, float(p.a) / 255.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (bDepth) glClear(GL_DEPTH_BUFFER_BIT);
		}

		void UpdateViewport(const alo::vi2d& pos, const alo::vi2d& size) override
		{
			glViewport(pos.x, pos.y, size.x, size.y);
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | END RENDERER: OpenGL 3.3 (3.0 es) (sh-sh-sh-shaders....)                     |
// O------------------------------------------------------------------------------O
#pragma endregion

// O------------------------------------------------------------------------------O
// | aloGameEngine Image loaders                                                  |
// O------------------------------------------------------------------------------O

#pragma region image_gdi
// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: GDI+, Windows Only, always exists, a little slow         |
// O------------------------------------------------------------------------------O
#if defined(ALO_IMAGE_GDI)

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#include <objidl.h>
#include <gdiplus.h>
#if defined(__MINGW32__) // Thanks Gusgo & Dandistine, but c'mon mingw!! wtf?!
	#include <gdiplus/gdiplusinit.h>
#else
	#include <gdiplusinit.h>
#endif
#include <shlwapi.h>
#undef min
#undef max

#if !defined(__MINGW32__)
	#pragma comment(lib, "gdiplus.lib")
	#pragma comment(lib, "Shlwapi.lib")
#endif

namespace alo
{
	static class GDIPlusStartup
	{
	public:
		GDIPlusStartup()
		{			
			Gdiplus::GdiplusStartupInput startupInput;
			GdiplusStartup(&token, &startupInput, NULL);
		}

		ULONG_PTR	token;
		
		~GDIPlusStartup()
		{
			// Well, MarcusTU thought this was important :D
			Gdiplus::GdiplusShutdown(token);
		}
	} gdistartup;

	class ImageLoader_GDIPlus : public alo::ImageLoader
	{
	private:
		std::wstring ConvertS2W(std::string s)
		{
#ifdef __MINGW32__
			wchar_t* buffer = new wchar_t[s.length() + 1];
			mbstowcs(buffer, s.c_str(), s.length());
			buffer[s.length()] = L'\0';
#else
			int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
			wchar_t* buffer = new wchar_t[count];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
			std::wstring w(buffer);
			delete[] buffer;
			return w;
		}

	public:
		ImageLoader_GDIPlus() : ImageLoader()
		{}

		alo::rcode LoadImageResource(alo::Sprite* spr, const std::string& sImageFile, alo::ResourcePack* pack) override
		{
			// clear out existing sprite
			spr->pColData.clear();

			// Open file
			UNUSED(pack);
			Gdiplus::Bitmap* bmp = nullptr;
			if (pack != nullptr)
			{
				// Load sprite from input stream
				ResourceBuffer rb = pack->GetFileBuffer(sImageFile);
				bmp = Gdiplus::Bitmap::FromStream(SHCreateMemStream((BYTE*)rb.vMemory.data(), UINT(rb.vMemory.size())));
			}
			else
			{
				// Check file exists
				if (!_gfs::exists(sImageFile)) return alo::rcode::NO_FILE;

				// Load sprite from file
				bmp = Gdiplus::Bitmap::FromFile(ConvertS2W(sImageFile).c_str());
			}

			if (bmp->GetLastStatus() != Gdiplus::Ok) return alo::rcode::FAIL;
			spr->width = bmp->GetWidth();
			spr->height = bmp->GetHeight();

			spr->pColData.resize(spr->width * spr->height);

			for (int y = 0; y < spr->height; y++)
				for (int x = 0; x < spr->width; x++)
				{
					Gdiplus::Color c;
					bmp->GetPixel(x, y, &c);
					spr->SetPixel(x, y, alo::Pixel(c.GetRed(), c.GetGreen(), c.GetBlue(), c.GetAlpha()));
				}
			delete bmp;
			return alo::rcode::OK;
		}

		alo::rcode SaveImageResource(alo::Sprite* spr, const std::string& sImageFile) override
		{
			return alo::rcode::OK;
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | END IMAGE LOADER: GDI+                                                       |
// O------------------------------------------------------------------------------O
#pragma endregion

#pragma region image_libpng
// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: libpng, default on linux, requires -lpng  (libpng-dev)   |
// O------------------------------------------------------------------------------O
#if defined(ALO_IMAGE_LIBPNG)
#include <png.h>
namespace alo
{
	void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length)
	{
		png_voidp a = png_get_io_ptr(pngPtr);
		((std::istream*)a)->read((char*)data, length);
	}

	class ImageLoader_LibPNG : public alo::ImageLoader
	{
	public:
		ImageLoader_LibPNG() : ImageLoader()
		{}

		alo::rcode LoadImageResource(alo::Sprite* spr, const std::string& sImageFile, alo::ResourcePack* pack) override
		{
			UNUSED(pack);

			// clear out existing sprite
			spr->pColData.clear();

			////////////////////////////////////////////////////////////////////////////
			// Use libpng, Thanks to Guillaume Cottenceau
			// https://gist.github.com/niw/5963798
			// Also reading png from streams
			// http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
			png_structp png;
			png_infop info;

			auto loadPNG = [&]()
			{
				png_read_info(png, info);
				png_byte color_type;
				png_byte bit_depth;
				png_bytep* row_pointers;
				spr->width = png_get_image_width(png, info);
				spr->height = png_get_image_height(png, info);
				color_type = png_get_color_type(png, info);
				bit_depth = png_get_bit_depth(png, info);
				if (bit_depth == 16) png_set_strip_16(png);
				if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
				if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)	png_set_expand_gray_1_2_4_to_8(png);
				if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
				if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
					png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
				if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
					png_set_gray_to_rgb(png);
				png_read_update_info(png, info);
				row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * spr->height);
				for (int y = 0; y < spr->height; y++) {
					row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
				}
				png_read_image(png, row_pointers);
				////////////////////////////////////////////////////////////////////////////
				// Create sprite array
				spr->pColData.resize(spr->width * spr->height);
				// Iterate through image rows, converting into sprite format
				for (int y = 0; y < spr->height; y++)
				{
					png_bytep row = row_pointers[y];
					for (int x = 0; x < spr->width; x++)
					{
						png_bytep px = &(row[x * 4]);
						spr->SetPixel(x, y, Pixel(px[0], px[1], px[2], px[3]));
					}
				}

				for (int y = 0; y < spr->height; y++) // Thanks maksym33
					free(row_pointers[y]);
				free(row_pointers);
				png_destroy_read_struct(&png, &info, nullptr);
			};

			png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if (!png) goto fail_load;

			info = png_create_info_struct(png);
			if (!info) goto fail_load;

			if (setjmp(png_jmpbuf(png))) goto fail_load;

			if (pack == nullptr)
			{
				FILE* f = fopen(sImageFile.c_str(), "rb");
				if (!f) return alo::rcode::NO_FILE;
				png_init_io(png, f);
				loadPNG();
				fclose(f);
			}
			else
			{
				ResourceBuffer rb = pack->GetFileBuffer(sImageFile);
				std::istream is(&rb);
				png_set_read_fn(png, (png_voidp)&is, pngReadStream);
				loadPNG();
			}

			return alo::rcode::OK;

		fail_load:
			spr->width = 0;
			spr->height = 0;
			spr->pColData.clear();
			return alo::rcode::FAIL;
		}

		alo::rcode SaveImageResource(alo::Sprite* spr, const std::string& sImageFile) override
		{
			return alo::rcode::OK;
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | END IMAGE LOADER:                                                            |
// O------------------------------------------------------------------------------O
#pragma endregion

#pragma region image_stb
// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: stb_image.h, all systems, very fast                      |
// O------------------------------------------------------------------------------O

// Note you need to download the above file into your project folder, and
// #define ALO_IMAGE_STB 
// #define ALO_GE_APPLICATION
// #include "aloGameEngine.h"

#if defined(ALO_IMAGE_STB)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace alo
{
	class ImageLoader_STB : public alo::ImageLoader
	{
	public:
		ImageLoader_STB() : ImageLoader()
		{}

		alo::rcode LoadImageResource(alo::Sprite* spr, const std::string& sImageFile, alo::ResourcePack* pack) override
		{
			UNUSED(pack);
			// clear out existing sprite
			spr->pColData.clear();
			// Open file
			stbi_uc* bytes = nullptr;
			int w = 0, h = 0, cmp = 0;
			if (pack != nullptr)
			{
				ResourceBuffer rb = pack->GetFileBuffer(sImageFile);
				bytes = stbi_load_from_memory((unsigned char*)rb.vMemory.data(), rb.vMemory.size(), &w, &h, &cmp, 4);
			}
			else
			{
				// Check file exists
				if (!_gfs::exists(sImageFile)) return alo::rcode::NO_FILE;
				bytes = stbi_load(sImageFile.c_str(), &w, &h, &cmp, 4);
			}

			if (!bytes) return alo::rcode::FAIL;
			spr->width = w; spr->height = h;
			spr->pColData.resize(spr->width * spr->height);
			std::memcpy(spr->pColData.data(), bytes, spr->width * spr->height * 4);
			delete[] bytes;
			return alo::rcode::OK;
		}

		alo::rcode SaveImageResource(alo::Sprite* spr, const std::string& sImageFile) override
		{
			return alo::rcode::OK;
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: stb_image.h                                              |
// O------------------------------------------------------------------------------O
#pragma endregion

// O------------------------------------------------------------------------------O
// | aloGameEngine Platforms                                                      |
// O------------------------------------------------------------------------------O

#pragma region platform_windows
// O------------------------------------------------------------------------------O
// | START PLATFORM: MICROSOFT WINDOWS XP, VISTA, 7, 8, 10                        |
// O------------------------------------------------------------------------------O
#if defined(ALO_PLATFORM_WINAPI)

#if defined(_WIN32) && !defined(__MINGW32__)
	#pragma comment(lib, "user32.lib")		// Visual Studio Only
	#pragma comment(lib, "gdi32.lib")		// For other Windows Compilers please add
	#pragma comment(lib, "opengl32.lib")	// these libs to your linker input
#endif

namespace alo
{
	class Platform_Windows : public alo::Platform
	{
	private:
		HWND olc_hWnd = nullptr;
		std::wstring wsAppName;

		std::wstring ConvertS2W(std::string s)
		{
#ifdef __MINGW32__
			wchar_t* buffer = new wchar_t[s.length() + 1];
			mbstowcs(buffer, s.c_str(), s.length());
			buffer[s.length()] = L'\0';
#else
			int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
			wchar_t* buffer = new wchar_t[count];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
			std::wstring w(buffer);
			delete[] buffer;
			return w;
		}

	public:
		virtual alo::rcode ApplicationStartUp() override { return alo::rcode::OK; }
		virtual alo::rcode ApplicationCleanUp() override { return alo::rcode::OK; }
		virtual alo::rcode ThreadStartUp() override { return alo::rcode::OK; }

		virtual alo::rcode ThreadCleanUp() override
		{
			renderer->DestroyDevice();
			PostMessage(olc_hWnd, WM_DESTROY, 0, 0);
			return alo::OK;
		}

		virtual alo::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const alo::vi2d& vViewPos, const alo::vi2d& vViewSize) override
		{
			if (renderer->CreateDevice({ olc_hWnd }, bFullScreen, bEnableVSYNC) == alo::rcode::OK)
			{
				renderer->UpdateViewport(vViewPos, vViewSize);
				return alo::rcode::OK;
			}
			else
				return alo::rcode::FAIL;
		}

		virtual alo::rcode CreateWindowPane(const alo::vi2d& vWindowPos, alo::vi2d& vWindowSize, bool bFullScreen) override
		{
			WNDCLASS wc;
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.lpfnWndProc = alo_WindowEvent;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.lpszMenuName = nullptr;
			wc.hbrBackground = nullptr;
			wc.lpszClassName = aloT("ALO_GAME_ENGINE");
			RegisterClass(&wc);

			// Define window furniture
			DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

			alo::vi2d vTopLeft = vWindowPos;

			// Handle Fullscreen
			if (bFullScreen)
			{
				dwExStyle = 0;
				dwStyle = WS_VISIBLE | WS_POPUP;
				HMONITOR hmon = MonitorFromWindow(olc_hWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi = { sizeof(mi) };
				if (!GetMonitorInfo(hmon, &mi)) return alo::rcode::FAIL;
				vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
				vTopLeft.x = 0;
				vTopLeft.y = 0;
			}

			// Keep client size as requested
			RECT rWndRect = { 0, 0, vWindowSize.x, vWindowSize.y };
			AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
			int width = rWndRect.right - rWndRect.left;
			int height = rWndRect.bottom - rWndRect.top;

			olc_hWnd = CreateWindowEx(dwExStyle, aloT("ALO_GAME_ENGINE"), aloT(""), dwStyle,
				vTopLeft.x, vTopLeft.y, width, height, NULL, NULL, GetModuleHandle(nullptr), this);

			// Create Keyboard Mapping
			mapKeys[0x00] = Key::NONE;
			mapKeys[0x41] = Key::A; mapKeys[0x42] = Key::B; mapKeys[0x43] = Key::C; mapKeys[0x44] = Key::D; mapKeys[0x45] = Key::E;
			mapKeys[0x46] = Key::F; mapKeys[0x47] = Key::G; mapKeys[0x48] = Key::H; mapKeys[0x49] = Key::I; mapKeys[0x4A] = Key::J;
			mapKeys[0x4B] = Key::K; mapKeys[0x4C] = Key::L; mapKeys[0x4D] = Key::M; mapKeys[0x4E] = Key::N; mapKeys[0x4F] = Key::O;
			mapKeys[0x50] = Key::P; mapKeys[0x51] = Key::Q; mapKeys[0x52] = Key::R; mapKeys[0x53] = Key::S; mapKeys[0x54] = Key::T;
			mapKeys[0x55] = Key::U; mapKeys[0x56] = Key::V; mapKeys[0x57] = Key::W; mapKeys[0x58] = Key::X; mapKeys[0x59] = Key::Y;
			mapKeys[0x5A] = Key::Z;

			mapKeys[VK_F1] = Key::F1; mapKeys[VK_F2] = Key::F2; mapKeys[VK_F3] = Key::F3; mapKeys[VK_F4] = Key::F4;
			mapKeys[VK_F5] = Key::F5; mapKeys[VK_F6] = Key::F6; mapKeys[VK_F7] = Key::F7; mapKeys[VK_F8] = Key::F8;
			mapKeys[VK_F9] = Key::F9; mapKeys[VK_F10] = Key::F10; mapKeys[VK_F11] = Key::F11; mapKeys[VK_F12] = Key::F12;

			mapKeys[VK_DOWN] = Key::DOWN; mapKeys[VK_LEFT] = Key::LEFT; mapKeys[VK_RIGHT] = Key::RIGHT; mapKeys[VK_UP] = Key::UP;
			//mapKeys[VK_RETURN] = Key::ENTER;// mapKeys[VK_RETURN] = Key::RETURN;
			
			mapKeys[VK_BACK] = Key::BACK; mapKeys[VK_ESCAPE] = Key::ESCAPE; mapKeys[VK_RETURN] = Key::ENTER; mapKeys[VK_PAUSE] = Key::PAUSE;
			mapKeys[VK_SCROLL] = Key::SCROLL; mapKeys[VK_TAB] = Key::TAB; mapKeys[VK_DELETE] = Key::DEL; mapKeys[VK_HOME] = Key::HOME;
			mapKeys[VK_END] = Key::END; mapKeys[VK_PRIOR] = Key::PGUP; mapKeys[VK_NEXT] = Key::PGDN; mapKeys[VK_INSERT] = Key::INS;
			mapKeys[VK_SHIFT] = Key::SHIFT; mapKeys[VK_CONTROL] = Key::CTRL;
			mapKeys[VK_SPACE] = Key::SPACE;

			mapKeys[0x30] = Key::K0; mapKeys[0x31] = Key::K1; mapKeys[0x32] = Key::K2; mapKeys[0x33] = Key::K3; mapKeys[0x34] = Key::K4;
			mapKeys[0x35] = Key::K5; mapKeys[0x36] = Key::K6; mapKeys[0x37] = Key::K7; mapKeys[0x38] = Key::K8; mapKeys[0x39] = Key::K9;

			mapKeys[VK_NUMPAD0] = Key::NP0; mapKeys[VK_NUMPAD1] = Key::NP1; mapKeys[VK_NUMPAD2] = Key::NP2; mapKeys[VK_NUMPAD3] = Key::NP3; mapKeys[VK_NUMPAD4] = Key::NP4;
			mapKeys[VK_NUMPAD5] = Key::NP5; mapKeys[VK_NUMPAD6] = Key::NP6; mapKeys[VK_NUMPAD7] = Key::NP7; mapKeys[VK_NUMPAD8] = Key::NP8; mapKeys[VK_NUMPAD9] = Key::NP9;
			mapKeys[VK_MULTIPLY] = Key::NP_MUL; mapKeys[VK_ADD] = Key::NP_ADD; mapKeys[VK_DIVIDE] = Key::NP_DIV; mapKeys[VK_SUBTRACT] = Key::NP_SUB; mapKeys[VK_DECIMAL] = Key::NP_DECIMAL;

			// Thanks scripticuk
			mapKeys[VK_OEM_1] = Key::OEM_1;			// On US and UK keyboards this is the ';:' key
			mapKeys[VK_OEM_2] = Key::OEM_2;			// On US and UK keyboards this is the '/?' key
			mapKeys[VK_OEM_3] = Key::OEM_3;			// On US keyboard this is the '~' key
			mapKeys[VK_OEM_4] = Key::OEM_4;			// On US and UK keyboards this is the '[{' key
			mapKeys[VK_OEM_5] = Key::OEM_5;			// On US keyboard this is '\|' key.
			mapKeys[VK_OEM_6] = Key::OEM_6;			// On US and UK keyboards this is the ']}' key
			mapKeys[VK_OEM_7] = Key::OEM_7;			// On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
			mapKeys[VK_OEM_8] = Key::OEM_8;			// miscellaneous characters. Varies by keyboard
			mapKeys[VK_OEM_PLUS] = Key::EQUALS;		// the '+' key on any keyboard
			mapKeys[VK_OEM_COMMA] = Key::COMMA;		// the comma key on any keyboard
			mapKeys[VK_OEM_MINUS] = Key::MINUS;		// the minus key on any keyboard
			mapKeys[VK_OEM_PERIOD] = Key::PERIOD;	// the period key on any keyboard
			mapKeys[VK_CAPITAL] = Key::CAPS_LOCK;
			return alo::OK;
		}

		virtual alo::rcode SetWindowTitle(const std::string& s) override
		{
#ifdef UNICODE
			SetWindowText(olc_hWnd, ConvertS2W(s).c_str());
#else
			SetWindowText(olc_hWnd, s.c_str());
#endif
			return alo::OK;
		}

		virtual alo::rcode StartSystemEventLoop() override
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return alo::OK;
		}

		virtual alo::rcode HandleSystemEvent() override { return alo::rcode::FAIL; }

		// Windows Event Handler - this is statically connected to the windows event system
		static LRESULT CALLBACK alo_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
			{
				// Thanks @ForAbby (Discord)
				uint16_t x = lParam & 0xFFFF; uint16_t y = (lParam >> 16) & 0xFFFF;
				int16_t ix = *(int16_t*)&x;   int16_t iy = *(int16_t*)&y;
				ptrGE->alo_UpdateMouse(ix, iy);
				return 0;
			}
			case WM_SIZE:       ptrGE->alo_UpdateWindowSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);	return 0;
			case WM_MOUSEWHEEL:	ptrGE->alo_UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));           return 0;
			case WM_MOUSELEAVE: ptrGE->alo_UpdateMouseFocus(false);                                    return 0;
			case WM_SETFOCUS:	ptrGE->alo_UpdateKeyFocus(true);                                       return 0;
			case WM_KILLFOCUS:	ptrGE->alo_UpdateKeyFocus(false);                                      return 0;
			case WM_KEYDOWN:	ptrGE->alo_UpdateKeyState(mapKeys[wParam], true);                      return 0;
			case WM_KEYUP:		ptrGE->alo_UpdateKeyState(mapKeys[wParam], false);                     return 0;
			case WM_SYSKEYDOWN: ptrGE->alo_UpdateKeyState(mapKeys[wParam], true);						return 0;
			case WM_SYSKEYUP:	ptrGE->alo_UpdateKeyState(mapKeys[wParam], false);						return 0;
			case WM_LBUTTONDOWN:ptrGE->alo_UpdateMouseState(0, true);                                  return 0;
			case WM_LBUTTONUP:	ptrGE->alo_UpdateMouseState(0, false);                                 return 0;
			case WM_RBUTTONDOWN:ptrGE->alo_UpdateMouseState(1, true);                                  return 0;
			case WM_RBUTTONUP:	ptrGE->alo_UpdateMouseState(1, false);                                 return 0;
			case WM_MBUTTONDOWN:ptrGE->alo_UpdateMouseState(2, true);                                  return 0;
			case WM_MBUTTONUP:	ptrGE->alo_UpdateMouseState(2, false);                                 return 0;
			case WM_CLOSE:		ptrGE->alo_Terminate();                                                return 0;
			case WM_DESTROY:	PostQuitMessage(0); DestroyWindow(hWnd);								return 0;
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	};
}
#endif
// O------------------------------------------------------------------------------O
// | END PLATFORM: MICROSOFT WINDOWS XP, VISTA, 7, 8, 10                          |
// O------------------------------------------------------------------------------O
#pragma endregion 

#endif // Headless

// O------------------------------------------------------------------------------O
// | aloGameEngine Auto-Configuration                                             |
// O------------------------------------------------------------------------------O
#pragma region ge_config
namespace alo
{
	void GameEngine::alo_ConfigureSystem()
	{

#if !defined(ALO_PGE_HEADLESS)

#if defined(ALO_IMAGE_GDI)
		alo::Sprite::loader = std::make_unique<alo::ImageLoader_GDIPlus>();
#endif

#if defined(ALO_IMAGE_LIBPNG)
		alo::Sprite::loader = std::make_unique<alo::ImageLoader_LibPNG>();
#endif

#if defined(ALO_IMAGE_STB)
		alo::Sprite::loader = std::make_unique<alo::ImageLoader_STB>();
#endif

#if defined(ALO_IMAGE_CUSTOM_EX)
		alo::Sprite::loader = std::make_unique<ALO_IMAGE_CUSTOM_EX>();
#endif




#if defined(ALO_PLATFORM_WINAPI)
		platform = std::make_unique<alo::Platform_Windows>();
#endif

#if defined(ALO_PLATFORM_X11)
		platform = std::make_unique<alo::Platform_Linux>();
#endif

#if defined(ALO_PLATFORM_GLUT)
		platform = std::make_unique<alo::Platform_GLUT>();
#endif

#if defined(ALO_PLATFORM_EMSCRIPTEN)
		platform = std::make_unique<alo::Platform_Emscripten>();
#endif

#if defined(ALO_PLATFORM_CUSTOM_EX)
		platform = std::make_unique<ALO_PLATFORM_CUSTOM_EX>();
#endif



#if defined(ALO_GFX_OPENGL10)
		renderer = std::make_unique<alo::Renderer_OGL10>();
#endif

#if defined(ALO_GFX_OPENGL33)
		renderer = std::make_unique<alo::Renderer_OGL33>();
#endif

#if defined(ALO_GFX_OPENGLES2)
		renderer = std::make_unique<alo::Renderer_OGLES2>();
#endif

#if defined(ALO_GFX_DIRECTX10)
		renderer = std::make_unique<alo::Renderer_DX10>();
#endif

#if defined(ALO_GFX_DIRECTX11)
		renderer = std::make_unique<alo::Renderer_DX11>();
#endif

#if defined(ALO_GFX_CUSTOM_EX)
		renderer = std::make_unique<ALO_RENDERER_CUSTOM_EX>();
#endif

		// Associate components with PGE instance
		platform->ptrGE = this;
		renderer->ptrGE = this;
#else
		alo::Sprite::loader = nullptr;
		platform = nullptr;
		renderer = nullptr;
#endif
	}
}

#pragma endregion

#endif // End ALO_GE_APPLICATION

// O------------------------------------------------------------------------------O
// | END OF ALO_GE_APPLICATION                                                   |
// O------------------------------------------------------------------------------O

