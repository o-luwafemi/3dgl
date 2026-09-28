// animation_ui.h
#pragma once
#include <GLFW/glfw3.h>
#include "ui.h"
#include "animator.h"

class AnimationUI
{
public:
	AnimationUI(int screenWidth, int screenHeight)
		: m_Renderer(screenWidth, screenHeight)
	{
		Layout(screenWidth, screenHeight);
	}

	void Resize(int screenWidth, int screenHeight)
	{
		m_Renderer.Resize(screenWidth, screenHeight);
		Layout(screenWidth, screenHeight);
	}

	// Call once per frame, before rendering the 3D scene or after — order doesn't matter
	// as long as UI is drawn last (see Render()).
	void Update(GLFWwindow* window, Animator& animator)
	{
		double mx, my;
		glfwGetCursorPos(window, &mx, &my);
		// std::cout << "mouse: " << mx << ", " << my << std::endl; // temporary
		// std::cout << "cursor: (" << mx << ", " << my << ")  playPauseBtn: ("
        //   << m_PlayPauseBtn.x << ", " << m_PlayPauseBtn.y << ", "
        //   << m_PlayPauseBtn.w << ", " << m_PlayPauseBtn.h << ")" << std::endl;
		bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool justPressed = mouseDown && !m_MouseWasDown;

		// Play/Pause button
		if (justPressed && HitTest(m_PlayPauseBtn, mx, my))
			animator.TogglePlayPause();

		// Stop button
		if (justPressed && HitTest(m_StopBtn, mx, my))
			animator.Stop();

		// Scrubber: click-drag anywhere on the track/handle
		bool overScrubber = HitTest(m_ScrubTrack, mx, my) || HitTest(m_ScrubHandleRect(animator), mx, my);
		if (justPressed && overScrubber)
			m_Dragging = true;
		if (!mouseDown)
			m_Dragging = false;

		if (m_Dragging)
		{
			float t = (float)(mx - m_ScrubTrack.x) / m_ScrubTrack.w;
			t = glm::clamp(t, 0.0f, 1.0f);
			animator.SeekNormalized(t);
			animator.Pause(); // dragging pauses playback, like a video scrubber
		}

		m_MouseWasDown = mouseDown;
		m_HoverPlayPause = HitTest(m_PlayPauseBtn, mx, my);
		m_HoverStop = HitTest(m_StopBtn, mx, my);
	}

	// Call after your 3D scene render, so UI draws on top
	void Render(Animator& animator)
	{
		m_Renderer.BeginFrame();

		glm::vec4 btnColor(0.2f, 0.2f, 0.2f, 0.9f);
		glm::vec4 btnHover(0.35f, 0.35f, 0.35f, 0.9f);
		glm::vec4 iconColor(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 trackColor(0.15f, 0.15f, 0.15f, 0.9f);
		glm::vec4 handleColor(0.9f, 0.6f, 0.1f, 1.0f);

		// Play/Pause button background
		m_Renderer.DrawQuad(m_PlayPauseBtn.x, m_PlayPauseBtn.y, m_PlayPauseBtn.w, m_PlayPauseBtn.h,
			m_HoverPlayPause ? btnHover : btnColor);

		// Icon: triangle if paused (show "play" prompt), two bars if playing (show "pause" prompt)
		float pad = m_PlayPauseBtn.h * 0.3f;
		if (!animator.IsPlaying())
		{
			m_Renderer.DrawTriangle(m_PlayPauseBtn.x + pad, m_PlayPauseBtn.y + pad,
				m_PlayPauseBtn.w - 2 * pad, m_PlayPauseBtn.h - 2 * pad, iconColor);
		}
		else
		{
			float barW = (m_PlayPauseBtn.w - 2 * pad) * 0.35f;
			float barH = m_PlayPauseBtn.h - 2 * pad;
			m_Renderer.DrawQuad(m_PlayPauseBtn.x + pad, m_PlayPauseBtn.y + pad, barW, barH, iconColor);
			m_Renderer.DrawQuad(m_PlayPauseBtn.x + m_PlayPauseBtn.w - pad - barW, m_PlayPauseBtn.y + pad,
				barW, barH, iconColor);
		}

		// Stop button
		m_Renderer.DrawQuad(m_StopBtn.x, m_StopBtn.y, m_StopBtn.w, m_StopBtn.h,
			m_HoverStop ? btnHover : btnColor);
		m_Renderer.DrawQuad(m_StopBtn.x + pad, m_StopBtn.y + pad,
			m_StopBtn.w - 2 * pad, m_StopBtn.h - 2 * pad, iconColor);

		// Scrubber track
		m_Renderer.DrawQuad(m_ScrubTrack.x, m_ScrubTrack.y, m_ScrubTrack.w, m_ScrubTrack.h, trackColor);

		// Scrubber handle
		Rect handle = m_ScrubHandleRect(animator);
		m_Renderer.DrawQuad(handle.x, handle.y, handle.w, handle.h, handleColor);

		m_Renderer.EndFrame();
	}

private:
	struct Rect { float x, y, w, h; };

	static bool HitTest(const Rect& r, double mx, double my)
	{
		return mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h;
	}

	Rect m_ScrubHandleRect(Animator& animator)
	{
		float t = animator.GetNormalizedTime();
		float handleW = 12.0f;
		float cx = m_ScrubTrack.x + t * m_ScrubTrack.w - handleW * 0.5f;
		return { cx, m_ScrubTrack.y - 4.0f, handleW, m_ScrubTrack.h + 8.0f };
	}

	void Layout(int screenWidth, int screenHeight)
	{
		float barHeight = 60.0f;
		float y = screenHeight - barHeight - 20.0f;
		float btnSize = 36.0f;
		float margin = 15.0f;

		m_PlayPauseBtn = { margin, y, btnSize, btnSize };
		m_StopBtn = { margin * 2 + btnSize, y, btnSize, btnSize };

		float scrubX = margin * 3 + btnSize * 2;
		m_ScrubTrack = { scrubX, y + btnSize * 0.5f - 3.0f, (float)screenWidth - scrubX - margin, 6.0f };
	}

	UIRenderer m_Renderer;
	Rect m_PlayPauseBtn, m_StopBtn, m_ScrubTrack;
	bool m_MouseWasDown = false;
	bool m_Dragging = false;
	bool m_HoverPlayPause = false;
	bool m_HoverStop = false;
};