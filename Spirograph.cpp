#define ALO_GE_APPLICATION
#include "aloGameEngine.h"

#define ALO_GEX_QUICKGUI
#include "aloGEX_QuickGUI.h"

namespace alo
{
	class Palette
	{
	public:
		enum class Stock
		{
			Empty,
			Greyscale,
			ColdHot,
			Spectrum,
		};

	public:
		Palette(const Stock stock = Stock::Empty)
		{
			switch (stock)
			{
			case Stock::Empty:
				vColors.clear();
				break;
			case Stock::Greyscale:
				vColors =
				{
					{0.0, alo::BLACK}, {1.0, alo::WHITE}
				};
				break;
			case Stock::ColdHot:
				vColors =
				{
					{0.0, alo::CYAN}, {0.5, alo::BLACK}, {1.0, alo::YELLOW}
				};
				break;
			case Stock::Spectrum:
				vColors =
				{
					{0.0 / 6.0, alo::DARK_BLUE},
					{1.0 / 6.0, alo::BLUE},
					{2.0 / 6.0, alo::CYAN},
					{3.0 / 6.0, alo::WHITE},
					{4.0 / 6.0, alo::GREEN},
					{5.0 / 6.0, alo::DARK_GREEN},
					{6.0 / 6.0, alo::VERY_DARK_BLUE}
				};
				break;
			}
		}

	public:
		alo::Pixel Sample(const double t) const
		{
			// Return obvious sample values
			if (vColors.empty())
				return alo::BLACK;

			if (vColors.size() == 1)
				return vColors.front().second;

			// Iterate through color entries until we find the first entry
			// with a location greater than our sample point
			double i = std::fmod(t, 1.0); // std::clamp(t, 0.0, 1.0);
			auto it = vColors.begin();
			while (i > it->first)
				++it;

			// If that is the first entry, just return it
			if (it == std::begin(vColors))
				return it->second;
			else
			{
				// else get the preceeding entry, and lerp between the two
				// proportionally
				auto it_p = std::prev(it);
				return alo::PixelLerp(it_p->second, it->second,
					float((i - it_p->first) / (it->first - it_p->first)));
			}
		}

		void SetColour(const double d, const alo::Pixel col)
		{
			double i = std::clamp(d, 0.0, 1.0);

			// If d already exists, replace it
			auto it = std::find_if(vColors.begin(), vColors.end(),
				[&i](const std::pair<double, alo::Pixel>& p)
				{
					return p.first == i;
				});

			if (it != std::end(vColors))
			{
				// Palette entry was found, so replace color entry
				it->second = col;
			}
			else
			{
				// Palette entry not found, so add it, and sort palette vector
				vColors.push_back({ i, col });
				std::sort(vColors.begin(), vColors.end(),
					[](const std::pair<double, alo::Pixel>& p1, std::pair<double, alo::Pixel>& p2)
					{
						return p2.first > p1.first;
					});
			}
		}

	private:
		std::vector<std::pair<double, alo::Pixel>> vColors;
	};
}

class Example : public alo::GameEngine
{
public:
	Example()
	{
		sAppName = "Spirographs";
	}

	alo::QuickGUI::Manager guiManager;
	alo::QuickGUI::Slider* guiSlider1 = nullptr;
	alo::QuickGUI::Slider* guiSlider2 = nullptr;
	alo::QuickGUI::Slider* guiSlider3 = nullptr;
	alo::QuickGUI::Button* guiButton1 = nullptr;
	alo::QuickGUI::Button* guiButton2 = nullptr;
	alo::QuickGUI::CheckBox* guiCheck1 = nullptr;

	alo::vf2d vOldPenPoint;
	bool bFirst = true;

	float fAccumulatedTime = 0.0f;

	alo::Palette p;


	// GE doesnt have a DrawCircleDEcal routine by default for a number
	// of reasons, so I've made one here that exploits a unit circle
	// being pre-made...
	std::vector<alo::vf2d> MakeUnitCircle(const size_t verts = 64)
	{
		std::vector<alo::vf2d> vOut(verts, { 0,0 });
		float anglestep = 2.0f * 3.14159f / float(verts - 1);
		for (size_t i = 0; i < verts; i++)
			vOut[i] = alo::vf2d(1.0f, anglestep * float(i)).cart();
		return vOut;
	}
	// ...see? It's here...						
	std::vector<alo::vf2d> vertsUnitCircle = MakeUnitCircle();
	// ...which means we only have to scale and offset the vertices when we want
	// to actually draw the circle, avoiding those trig functions
	void DrawCircleDecal(const alo::vf2d& vPos, const float fRadius, const alo::Pixel colour = alo::WHITE)
	{
		std::vector<alo::vf2d> vDraw(vertsUnitCircle.size(), { 0,0 });
		std::transform(vertsUnitCircle.begin(), vertsUnitCircle.end(), vDraw.begin(),
			[&](const alo::vf2d& vIn) { return vIn * fRadius + vPos; });
		SetDecalMode(alo::DecalMode::WIREFRAME);
		DrawPolygonDecal(nullptr, vDraw, vDraw, colour);
		SetDecalMode(alo::DecalMode::NORMAL);
	}

public:
	bool OnUserCreate() override
	{
		// Outer Gear Radius
		guiSlider1 = new alo::QuickGUI::Slider(guiManager,
			{ 1700.0f, 10.0f }, { 1900.0f, 10.0f }, 0, 400.0f, 200.0f);

		// Inner Gear Radius
		guiSlider2 = new alo::QuickGUI::Slider(guiManager,
			{ 1700.0f, 30.0f }, { 1900.0f, 30.0f }, -256.0f, 256.0f, 77.0f);

		// Pen Radius
		guiSlider3 = new alo::QuickGUI::Slider(guiManager,
			{ 1700.0f, 50.0f }, { 1900.0f, 50.0f }, 0, 256.0f, 65.0f);

		guiButton1 = new alo::QuickGUI::Button(guiManager,
			"Clear All", { 1700.0f, 80.0f }, { 100.0f, 16.0f });

		guiCheck1 = new alo::QuickGUI::CheckBox(guiManager,
			"Show Gears", true, { 1810.0f, 80.0f }, { 90.0f, 16.0f });

		guiButton2 = new alo::QuickGUI::Button(guiManager,
			"Draw!", { 1700.0f, 110.0f }, { 200.0f, 20.0f });

		p = alo::Palette(alo::Palette::Stock::Spectrum);

		Reset();
		return true;
	}

	// Revert to known state
	void Reset()
	{
		bFirst = true;
		fAccumulatedTime = 0.0f;
		Clear(alo::BLACK);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		guiManager.Update(this);

		// Reset the image when "R" key is pressed
		if (GetKey(alo::Key::R).bPressed || guiButton1->bPressed)
			Reset();

		// Advance "time" only when the user wishes to draw
		if (GetKey(alo::Key::SPACE).bHeld || guiButton2->bHeld)
			fAccumulatedTime += fElapsedTime * 5.0f;


		float fFixedGearRadius = guiSlider1->fValue;  // 400.0f;
		float fMovingGearRadius = guiSlider2->fValue; // 77.0f;
		float fPenOffsetRadius = guiSlider3->fValue;  // 65.0f;

		// Center point of inner gear
		alo::vf2d vMovingGearPos =
		{
			(fFixedGearRadius - fMovingGearRadius) * cos(fAccumulatedTime),
			(fFixedGearRadius - fMovingGearRadius) * sin(fAccumulatedTime)
		};

		// Determine gear ratio between gears
		float ratio = fFixedGearRadius / fMovingGearRadius;

		// Offset of pen from inner gear center, note direction is reversed!
		alo::vf2d vPenOffset =
		{
			(fPenOffsetRadius)*cos(-fAccumulatedTime * ratio),
			(fPenOffsetRadius)*sin(-fAccumulatedTime * ratio)
		};

		alo::vf2d vFixedGearPos = { 960.0f, 540.0f};
		alo::vf2d vPenPoint = vFixedGearPos + vMovingGearPos + vPenOffset;

		// Check if first point is being drawn, as we dont want to 
		// ruin the spirograph by starting from some old location
		if (bFirst)
		{
			vOldPenPoint = vPenPoint;
			bFirst = false;
		}

		// Draw the "gears"
		if (guiCheck1->bChecked)
		{
			// Draw as "Decals" so they appear on top of sprites
			DrawCircleDecal(vFixedGearPos, fFixedGearRadius, alo::WHITE);
			DrawCircleDecal(vFixedGearPos + vMovingGearPos, std::abs(fMovingGearRadius), alo::WHITE);
			DrawCircleDecal(vPenPoint, 4, alo::WHITE);
			DrawLineDecal(
				vFixedGearPos + vMovingGearPos + vPenOffset.norm() * fMovingGearRadius,
				vFixedGearPos + vMovingGearPos - vPenOffset.norm() * fMovingGearRadius, alo::WHITE);
		}

		// Draws the GUI
		guiManager.DrawDecal(this);

		// Sprite Draw a line from the previous pen point to the new one
		if (GetKey(alo::Key::SPACE).bHeld || guiButton2->bHeld)
			DrawLine(vOldPenPoint, vPenPoint, p.Sample(fAccumulatedTime / 300.0f));

		// Store old pen point
		vOldPenPoint = vPenPoint;
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(1920, 1080, 1, 1))
		demo.Start();
	return 0;
}