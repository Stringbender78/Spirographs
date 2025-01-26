#ifndef ALO_GEX_QUICKGUI_H
#define ALO_GEX_QUICKGUI_H

#include "aloGameEngine.h"


namespace alo::QuickGUI
{
	class Manager;

	// Virtual base class for all controls
	class BaseControl
	{
	public:
		BaseControl(alo::QuickGUI::Manager& manager);
		virtual ~BaseControl();

	public:
		// Switches the control on/off
		void Enable(const bool bEnable);
		// Sets whether or not the control is interactive/displayed
		bool bVisible = true;

		// True on single frame control begins being manipulated
		bool bPressed = false;
		// True on all frames control is under user manipulation
		bool bHeld = false;
		// True on single frame control ceases being manipulated
		bool bReleased = false;

	public:
		// Updates the controls behvaiour
		virtual void Update(alo::GameEngine* ge) = 0;
		// Draws the control using "sprite" based CPU operations
		virtual void Draw(alo::GameEngine* ge) = 0;
		// Draws the control using "decal" based GPU operations
		virtual void DrawDecal(alo::GameEngine* ge) = 0;

	protected:
		// Controls are related to a manager, where the theme resides
		// and control groups can be implemented
		alo::QuickGUI::Manager& m_manager;

		// All controls exists in one of four states
		// Disabled - Greyed out and not interactive
		// Normal - interactive and operational
		// Hover - currently under the users mouse focus
		// Click - user is interacting with the control
		enum class State { Disabled, Normal, Hover, Click } m_state = State::Normal;
		
		// To add a "swish" to things, controls can fade between states
		float m_fTransition = 0.0;
	};


	// A QuickGUI::Manager acts as a convenient grouping of controls
	class Manager
	{
	public:
		// Construct Manager, bCleanUpForMe will automatically DELETE any controls
		// given to this manager via AddControl() if true
		Manager(const bool bCleanUpForMe = true);
		virtual ~Manager();
	
	public:
		// Add a gui element derived form BaseControl to this manager
		void AddControl(BaseControl* control);
		// Updates all controls this manager operates
		void Update(alo::GameEngine* ge);
		// Draws as "sprite" all controls this manager operates
		void Draw(alo::GameEngine* ge);
		// Draws as "decal" all controls this manager operates
		void DrawDecal(alo::GameEngine* ge);
		
	public: // This managers "Theme" can be set here
		// Various element colors
		alo::Pixel colNormal = alo::DARK_BLUE;
		alo::Pixel colHover = alo::BLUE;
		alo::Pixel colClick = alo::CYAN;
		alo::Pixel colDisable = alo::DARK_GREY;
		alo::Pixel colBorder = alo::WHITE;
		alo::Pixel colText = alo::WHITE;
		// Speed to transiton from Normal -> Hover
		float fHoverSpeedOn = 10.0f;
		// Speed to transiton from Hover -> Normal
		float fHoverSpeedOff = 4.0f;
		// Size of grab handle
		float fGrabRad = 8.0f;
		// Copy all theme attributes into a different manager object
		void CopyThemeFrom(const Manager& manager);

	private:
		// Should this manager call delete on the controls it opeerates?
		bool m_bEraseControlsOnDestroy = true;
		// Container of controls
		std::vector<BaseControl*> m_vControls;
	};


	// Creates a Label Control - it's just text!
	class Label : public BaseControl
	{
	public:
		Label(alo::QuickGUI::Manager& manager,  // Associate with a Manager
			const std::string& text,			// Text to display
			const alo::vf2d& pos,				// Location of label top-left
			const alo::vf2d& size);				// Size of label

	public:
		// Position of button
		alo::vf2d vPos;
		// Size of button
		alo::vf2d vSize;
		// Text displayed on button
		std::string sText;
		// Show a border?
		bool bHasBorder = false;
		// Show a background?
		bool bHasBackground = false;
		// Where should the text be positioned?
		enum class Alignment
		{Left, Centre, Right} nAlign = Alignment::Centre;

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;
	};

	class TextBox : public Label
	{
	public:
		TextBox(alo::QuickGUI::Manager& manager,	// Associate with a Manager
			const std::string& text,				// Text to display
			const alo::vf2d& pos,					// Location of text box top-left
			const alo::vf2d& size);					// Size of text box

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;

	protected:
		bool m_bTextEdit = false;

	};

	// Creates a Button Control - a clickable, labelled rectangle
	class Button : public BaseControl
	{
	public:
		Button(alo::QuickGUI::Manager& manager, // Associate with a Manager
			const std::string& text,			// Text to display
			const alo::vf2d& pos,				// Location of button top-left
			const alo::vf2d& size);				// Size of button
				
	public:
		// Position of button
		alo::vf2d vPos;	
		// Size of button
		alo::vf2d vSize;
		// Text displayed on button
		std::string sText;

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;
	};

	// Creates a Button Control - a clickable, labelled rectangle
	class CheckBox : public Button
	{
	public:
		CheckBox(alo::QuickGUI::Manager& manager,	// Associate with a Manager
			const std::string& text,				// Text to display
			const bool check,						// Is checked or not?
			const alo::vf2d& pos,					// Location of button top-left
			const alo::vf2d& size);					// Size of button

	public:
		bool bChecked = false;

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;
	};


	// Creates a Slider Control - a grabbable handle that slides between two locations 
	class Slider : public BaseControl
	{
	public:
		Slider(alo::QuickGUI::Manager& manager, // Associate with a Manager
			const alo::vf2d& posmin,			// Screen location of "minimum"
			const alo::vf2d& posmax,			// Screen location of "maximum"
			const float valmin,					// Value of minimum
			const float valmax,					// Value of maximum
			const float value);					// Starting value

	public:
		// Minium value
		float fMin = -100.0f;
		// Maximum value
		float fMax = +100.0f;
		// Current value
		float fValue = 0.0f;

		// Location of minimum/start
		alo::vf2d vPosMin;
		// Location of maximum/end
		alo::vf2d vPosMax;

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;
	};


	class ListBox : public BaseControl
	{
	public:
		ListBox(alo::QuickGUI::Manager& manager,	// Associate with a Manager
			std::vector<std::string>& vList,			
			const alo::vf2d& pos,					// Location of list top-left
			const alo::vf2d& size);					// Size of list

		// Position of list
		alo::vf2d vPos;
		// Size of list
		alo::vf2d vSize;
		// Show a border?
		bool bHasBorder = true;
		// Show a background?
		bool bHasBackground = true;

	public:
		Slider *m_pSlider = nullptr;
		Manager m_group;
		size_t  m_nVisibleItems = 0;
		std::vector<std::string>& m_vList;

	public:
		size_t nSelectedItem = 0;

	public: // BaseControl overrides
		void Update(alo::GameEngine* ge) override;
		void Draw(alo::GameEngine* ge) override;
		void DrawDecal(alo::GameEngine* ge) override;
	};
}


#ifdef ALO_GEX_QUICKGUI
#undef ALO_GEX_QUICKGUI
namespace alo::QuickGUI
{

#pragma region BaseControl
	BaseControl::BaseControl(alo::QuickGUI::Manager& manager) : m_manager(manager)
	{
		m_manager.AddControl(this);
	}

	BaseControl::~BaseControl()
	{

	}

	void BaseControl::Enable(const bool bEnable)
	{
		m_state = bEnable ? State::Normal : State::Disabled;
	}
#pragma endregion

#pragma region Manager
	Manager::Manager(const bool bCleanUpForMe)
	{
		m_bEraseControlsOnDestroy = bCleanUpForMe;
	}

	Manager::~Manager()
	{
		if (m_bEraseControlsOnDestroy)
			for (auto& p : m_vControls)
				delete p;

		m_vControls.clear();
	}

	void Manager::AddControl(BaseControl* control)
	{
		m_vControls.push_back(control);
	}

	void Manager::Update(alo::GameEngine* ge)
	{
		for (auto& p : m_vControls) p->Update(ge);
	}

	void Manager::Draw(alo::GameEngine* ge)
	{
		for (auto& p : m_vControls) p->Draw(ge);
	}

	void Manager::DrawDecal(alo::GameEngine* ge)
	{
		for (auto& p : m_vControls) p->DrawDecal(ge);
	}

	void Manager::CopyThemeFrom(const Manager& manager)
	{
		this->colBorder = manager.colBorder;
		this->colClick = manager.colClick;
		this->colDisable = manager.colDisable;
		this->colHover = manager.colHover;
		this->colNormal = manager.colNormal;
		this->colText = manager.colText;
		this->fGrabRad = manager.fGrabRad;
		this->fHoverSpeedOff = manager.fHoverSpeedOff;
		this->fHoverSpeedOn = manager.fHoverSpeedOn;		
	}
#pragma endregion

#pragma region Label
	Label::Label(alo::QuickGUI::Manager& manager, const std::string& text, const alo::vf2d& pos, const alo::vf2d& size)
		: BaseControl(manager)
	{
		vPos = pos; vSize = size; sText = text;
	}
	
	void Label::Update(alo::GameEngine* ge)
	{

	}

	void Label::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
		{
			ge->FillRect(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);
		}

		if(bHasBorder)
			ge->DrawRect(vPos, vSize - alo::vf2d(1, 1), m_manager.colBorder);

		alo::vf2d vText = ge->GetTextSizeProp(sText);
		switch (nAlign)
		{
		case Alignment::Left:
			ge->DrawStringProp(alo::vf2d( vPos.x + 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f ), sText, m_manager.colText);
			break;
		case Alignment::Centre:
			ge->DrawStringProp(vPos + (vSize - vText) * 0.5f, sText, m_manager.colText);
			break;
		case Alignment::Right:
			ge->DrawStringProp(alo::vf2d{ vPos.x + vSize.x - vText.x - 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f }, sText, m_manager.colText);
			break;
		}
	}

	void Label::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
		{
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);
		}

		if (bHasBorder)
		{
			ge->SetDecalMode(alo::DecalMode::WIREFRAME);
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2,2), m_manager.colBorder);
			ge->SetDecalMode(alo::DecalMode::NORMAL);
		}

		alo::vf2d vText = ge->GetTextSizeProp(sText);
		switch (nAlign)
		{
		case Alignment::Left:
			ge->DrawStringPropDecal({ vPos.x + 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f }, sText, m_manager.colText);
			break;
		case Alignment::Centre:
			ge->DrawStringPropDecal(vPos + (vSize - vText) * 0.5f, sText, m_manager.colText);
			break;
		case Alignment::Right:
			ge->DrawStringPropDecal({ vPos.x + vSize.x - vText.x - 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f }, sText, m_manager.colText);
			break;
		}
	}
#pragma endregion


#pragma region TextBox
	TextBox::TextBox(alo::QuickGUI::Manager& manager, const std::string& text, const alo::vf2d& pos, const alo::vf2d& size)
		: Label(manager, text, pos, size)
	{
		nAlign = Alignment::Left;
		bHasBorder = true;
		bHasBackground = false;
	}

	void TextBox::Update(alo::GameEngine* ge)
	{
		if (m_state == State::Disabled || !bVisible)
			return;

		bPressed = false;
		bReleased = false;
	
		alo::vf2d vMouse = ge->GetMousePos();
		
		if (vMouse.x >= vPos.x && vMouse.x < vPos.x + vSize.x &&
			vMouse.y >= vPos.y && vMouse.y < vPos.y + vSize.y)
		{
			// Released inside box does nothing to me, but i may have
			// to finish off the neighbors... oo err
			bReleased = ge->GetMouse(alo::Mouse::LEFT).bReleased;
			if (bReleased && ge->IsTextEntryEnabled() && !m_bTextEdit)
			{
				ge->TextEntryEnable(false);
			}

			bPressed = ge->GetMouse(alo::Mouse::LEFT).bPressed;
			if (bPressed && !ge->IsTextEntryEnabled() && !m_bTextEdit)
			{				
				ge->TextEntryEnable(true, sText);
				m_bTextEdit = true;
			}

			bHeld = ge->GetMouse(alo::Mouse::LEFT).bHeld;

			
		}
		else
		{
			// Released outside box
			bReleased = ge->GetMouse(alo::Mouse::LEFT).bReleased;
			if (bReleased && m_bTextEdit)
			{
				sText = ge->TextEntryGetString();
				ge->TextEntryEnable(false);
				m_bTextEdit = false;
			}
		}	

		if (m_bTextEdit && ge->IsTextEntryEnabled())
			sText = ge->TextEntryGetString();
	}

	void TextBox::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
		{
			ge->FillRect(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);
		}

		if (bHasBorder)
			ge->DrawRect(vPos, vSize - alo::vf2d(1, 1), m_manager.colBorder);

		if (m_bTextEdit && ge->IsTextEntryEnabled())
		{
			// Draw Cursor
			int32_t i = ge->TextEntryGetCursor();
			alo::vf2d vCursorPos = ge->GetTextSizeProp(sText.substr(0, i));
			ge->FillRect(alo::vf2d(vPos.x + 2.0f + vCursorPos.x, (vPos.y + (vSize.y - 10.0f) * 0.5f)), { 2, 10 }, m_manager.colText);
		}
		
		// Draw Text
		alo::vf2d vText = ge->GetTextSizeProp(sText);
		ge->DrawStringProp(alo::vf2d(vPos.x + 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f), sText, m_manager.colText);
				
	}

	void TextBox::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
		{
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);
		}

		if (bHasBorder)
		{
			ge->SetDecalMode(alo::DecalMode::WIREFRAME);
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colBorder);
			ge->SetDecalMode(alo::DecalMode::NORMAL);
		}

		if (m_bTextEdit && ge->IsTextEntryEnabled())
		{
			// Draw Cursor
			int32_t i = ge->TextEntryGetCursor();
			alo::vf2d vCursorPos = ge->GetTextSizeProp(sText.substr(0, i));
			ge->FillRectDecal(alo::vf2d(vPos.x + 2.0f + vCursorPos.x, (vPos.y + (vSize.y - 10.0f) * 0.5f)), { 2, 10 }, m_manager.colText);
		}

		// Draw Text
		alo::vf2d vText = ge->GetTextSizeProp(sText);
		ge->DrawStringPropDecal(alo::vf2d(vPos.x + 2.0f, vPos.y + (vSize.y - vText.y) * 0.5f), sText, m_manager.colText);
	}
#pragma endregion

#pragma region Button
	Button::Button(alo::QuickGUI::Manager& manager, const std::string& text, const alo::vf2d& pos, const alo::vf2d& size) 
		: BaseControl(manager)
	{
		vPos = pos; vSize = size; sText = text;
	}

	void Button::Update(alo::GameEngine* ge)
	{
		if (m_state == State::Disabled || !bVisible)
			return;

		bPressed = false;
		bReleased = false;
		float fElapsedTime = ge->GetElapsedTime();

		alo::vf2d vMouse = ge->GetMousePos();
		if (m_state != State::Click)
		{
			if (vMouse.x >= vPos.x && vMouse.x < vPos.x + vSize.x &&
				vMouse.y >= vPos.y && vMouse.y < vPos.y + vSize.y)
			{
				m_fTransition += fElapsedTime * m_manager.fHoverSpeedOn;
				m_state = State::Hover;

				bPressed = ge->GetMouse(alo::Mouse::LEFT).bPressed;				
				if (bPressed)
				{
					m_state = State::Click;
				}

				bHeld = ge->GetMouse(alo::Mouse::LEFT).bHeld;
			}
			else
			{
				m_fTransition -= fElapsedTime * m_manager.fHoverSpeedOff;
				m_state = State::Normal;
			}
		}
		else
		{	
			bHeld = ge->GetMouse(alo::Mouse::LEFT).bHeld;
			bReleased = ge->GetMouse(alo::Mouse::LEFT).bReleased;
			if (bReleased) m_state = State::Normal;
		}

		m_fTransition = std::clamp(m_fTransition, 0.0f, 1.0f);
	}

	void Button::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		switch (m_state)
		{
		case State::Disabled:
			ge->FillRect(vPos, vSize, m_manager.colDisable);
			break;
		case State::Normal:
		case State::Hover:
			ge->FillRect(vPos, vSize, alo::PixelLerp(m_manager.colNormal, m_manager.colHover, m_fTransition));
			break;
		case State::Click:
			ge->FillRect(vPos, vSize, m_manager.colClick);
			break;
		}

		ge->DrawRect(vPos, vSize - alo::vf2d(1, 1), m_manager.colBorder);
		alo::vf2d vText = ge->GetTextSizeProp(sText);
		ge->DrawStringProp(vPos + (vSize - vText) * 0.5f, sText, m_manager.colText);
	}

	void Button::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		switch (m_state)
		{
		case State::Disabled:
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colDisable);
			break;
		case State::Normal:
		case State::Hover:
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), alo::PixelLerp(m_manager.colNormal, m_manager.colHover, m_fTransition));
			break;
		case State::Click:
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colClick);
			break;
		}
		ge->SetDecalMode(alo::DecalMode::WIREFRAME);
		ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colBorder);
		ge->SetDecalMode(alo::DecalMode::NORMAL);

		alo::vf2d vText = ge->GetTextSizeProp(sText);
		ge->DrawStringPropDecal(vPos + (vSize - vText) * 0.5f, sText, m_manager.colText);
	}
#pragma endregion


#pragma region CheckBox
	CheckBox::CheckBox(alo::QuickGUI::Manager& manager, const std::string& text, const bool check, const alo::vf2d& pos, const alo::vf2d& size)
		: Button(manager, text, pos, size)
	{
		bChecked = check;
	}

	void CheckBox::Update(alo::GameEngine* ge)
	{
		if (m_state == State::Disabled || !bVisible)
			return;

		Button::Update(ge);
		if (bPressed) bChecked = !bChecked;
	}

	void CheckBox::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		Button::Draw(ge);

		if (bChecked)
			ge->DrawRect(vPos + alo::vf2d(2, 2), vSize - alo::vi2d(5, 5), m_manager.colBorder);
	}

	void CheckBox::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		Button::DrawDecal(ge);

		ge->SetDecalMode(alo::DecalMode::WIREFRAME);
		ge->FillRectDecal(vPos + alo::vf2d(2,2), vSize - alo::vf2d(4, 4), m_manager.colBorder);
		ge->SetDecalMode(alo::DecalMode::NORMAL);

		alo::vf2d vText = ge->GetTextSizeProp(sText);
		ge->DrawStringPropDecal(vPos + (vSize - vText) * 0.5f, sText, m_manager.colText);
	}
#pragma endregion

#pragma region Slider
	Slider::Slider(alo::QuickGUI::Manager& manager, const alo::vf2d& posmin, const alo::vf2d& posmax, const float valmin, const float valmax, const float value) 
		: BaseControl(manager)
	{
		vPosMin = posmin; vPosMax = posmax; fMin = valmin; fMax = valmax; fValue = value;
	}

	void Slider::Update(alo::GameEngine* ge)
	{
		if (m_state == State::Disabled || !bVisible)
			return;
		
		float fElapsedTime = ge->GetElapsedTime();

		alo::vf2d vMouse = ge->GetMousePos();
		bHeld = false;
		if (m_state == State::Click)
		{
			alo::vf2d d = vPosMax - vPosMin;
			float u = d.dot(vMouse - vPosMin) / d.mag2();
			fValue = u * (fMax - fMin) + fMin;
			bHeld = true;
		}
		else
		{
			alo::vf2d vSliderPos = vPosMin + (vPosMax - vPosMin) * ((fValue - fMin) / (fMax - fMin));
			if ((vMouse - vSliderPos).mag2() <= int32_t(m_manager.fGrabRad) * int32_t(m_manager.fGrabRad))
			{
				m_fTransition += fElapsedTime * m_manager.fHoverSpeedOn;
				m_state = State::Hover;
				if (ge->GetMouse(alo::Mouse::LEFT).bPressed)
				{
					m_state = State::Click;
					bPressed = true;
				}
			}
			else
				m_state = State::Normal;
		}

		if (ge->GetMouse(alo::Mouse::LEFT).bReleased)
		{
			m_state = State::Normal;
			bReleased = true;
		}

		if (m_state == State::Normal)
		{
			m_fTransition -= fElapsedTime * m_manager.fHoverSpeedOff;
			m_state = State::Normal;
			bHeld = false;		
		}

		fValue = std::clamp(fValue, fMin, fMax);
		m_fTransition = std::clamp(m_fTransition, 0.0f, 1.0f);
	}

	void Slider::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		ge->DrawLine(vPosMin, vPosMax, m_manager.colBorder);
		alo::vf2d vSliderPos = vPosMin + (vPosMax - vPosMin) * ((fValue - fMin) / (fMax - fMin));

		switch (m_state)
		{
		case State::Disabled:
			ge->FillCircle(vSliderPos, int32_t(m_manager.fGrabRad), m_manager.colDisable);
			break;
		case State::Normal:
		case State::Hover:
			ge->FillCircle(vSliderPos, int32_t(m_manager.fGrabRad), alo::PixelLerp(m_manager.colNormal, m_manager.colHover, m_fTransition));
			break;
		case State::Click:
			ge->FillCircle(vSliderPos, int32_t(m_manager.fGrabRad), m_manager.colClick);
			break;
		}


		ge->DrawCircle(vSliderPos, int32_t(m_manager.fGrabRad), m_manager.colBorder);
	}

	void Slider::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		ge->DrawLineDecal(vPosMin, vPosMax, m_manager.colBorder);
		alo::vf2d vSliderPos = vPosMin + (vPosMax - vPosMin) * ((fValue - fMin) / (fMax - fMin));

		switch (m_state)
		{
		case State::Disabled:
			ge->FillRectDecal(vSliderPos - alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad), alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad) * 2.0f, m_manager.colDisable);
			break;
		case State::Normal:
		case State::Hover:
			ge->FillRectDecal(vSliderPos - alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad), alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad) * 2.0f, alo::PixelLerp(m_manager.colNormal, m_manager.colHover, m_fTransition));
			break;
		case State::Click:
			ge->FillRectDecal(vSliderPos - alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad), alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad) * 2.0f, m_manager.colClick);
			break;
		}

		ge->SetDecalMode(alo::DecalMode::WIREFRAME);
		ge->FillRectDecal(vSliderPos - alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad), alo::vf2d(m_manager.fGrabRad, m_manager.fGrabRad) * 2.0f, m_manager.colBorder);
		ge->SetDecalMode(alo::DecalMode::NORMAL);
	}


#pragma endregion

#pragma region ListBox
	ListBox::ListBox(alo::QuickGUI::Manager& manager, std::vector<std::string>& vList, const alo::vf2d& pos, const alo::vf2d& size)
		: BaseControl(manager), m_vList(vList)
	{
		m_group.CopyThemeFrom(m_manager);
		vPos = pos;
		vSize = size;
		m_pSlider = new Slider(m_group, { pos.x + size.x - m_manager.fGrabRad - 1, pos.y + m_manager.fGrabRad + 1 },
			{ pos.x + size.x - m_manager.fGrabRad - 1, pos.y + size.y - m_manager.fGrabRad - 1 }, 0, float(m_vList.size()), 0);		
	}

	void ListBox::Update(alo::GameEngine* ge)
	{
		if (m_state == State::Disabled || !bVisible)
			return;

		alo::vf2d vMouse = ge->GetMousePos() - vPos + alo::vi2d(2,0);
		if (ge->GetMouse(alo::Mouse::LEFT).bPressed)
		{
			if (vMouse.x >= 0 && vMouse.x < vSize.x - (m_group.fGrabRad * 2) && vMouse.y >= 0 && vMouse.y < vSize.y)
			{
				nSelectedItem = size_t(m_pSlider->fValue + vMouse.y / 10);
			}
		}

		nSelectedItem = std::clamp(nSelectedItem, size_t(0), m_vList.size()-1);

		m_pSlider->fMax = float(m_vList.size());
		m_group.Update(ge);		
	}

	void ListBox::Draw(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
		{
			ge->FillRect(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);
		}

		if (bHasBorder)
			ge->DrawRect(vPos, vSize - alo::vf2d(1, 1), m_manager.colBorder);


		size_t idx0 = size_t(m_pSlider->fValue);
		size_t idx1 = std::min(idx0 + size_t((vSize.y - 4) / 10), m_vList.size());

		alo::vf2d vTextPos = vPos + alo::vf2d(2,2);
		for (size_t idx = idx0; idx < idx1; idx++)
		{
			if (idx == nSelectedItem)
				ge->FillRect(vTextPos - alo::vi2d(1,1), {int32_t(vSize.x - m_group.fGrabRad * 2), 10}, m_group.colHover);
			ge->DrawStringProp(vTextPos, m_vList[idx]);
			vTextPos.y += 10;
		}

		m_group.Draw(ge);
	}

	void ListBox::DrawDecal(alo::GameEngine* ge)
	{
		if (!bVisible)
			return;

		if (bHasBackground)
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colNormal);

		size_t idx0 = size_t(m_pSlider->fValue);
		size_t idx1 = std::min(idx0 + size_t((vSize.y - 4) / 10), m_vList.size());

		alo::vf2d vTextPos = vPos + alo::vf2d(2, 2);
		for (size_t idx = idx0; idx < idx1; idx++)
		{
			if (idx == nSelectedItem)
				ge->FillRectDecal(vTextPos - alo::vi2d(1, 1), { vSize.x - m_group.fGrabRad * 2.0f, 10.0f }, m_group.colHover);
			ge->DrawStringPropDecal(vTextPos, m_vList[idx]);
			vTextPos.y += 10;
		}

		if (bHasBorder)
		{
			ge->SetDecalMode(alo::DecalMode::WIREFRAME);
			ge->FillRectDecal(vPos + alo::vf2d(1, 1), vSize - alo::vf2d(2, 2), m_manager.colBorder);
			ge->SetDecalMode(alo::DecalMode::NORMAL);
		}

		m_group.DrawDecal(ge);
	}
#pragma endregion


}
#endif // ALO_GEX_QUICKGUI
#endif // ALO_GEX_QUICKGUI_H