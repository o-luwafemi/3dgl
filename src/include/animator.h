#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
		m_IsPlaying = true;
		m_PlaybackSpeed = 1.0f;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation && m_IsPlaying)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt * m_PlaybackSpeed;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		}

		if(m_CurrentAnimation)
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
		m_IsPlaying = true;
	}


	// --- new playback controls ---
	void Play()  { m_IsPlaying = true; }
	void Pause() { m_IsPlaying = false; }
	void Stop()  { m_IsPlaying = false; m_CurrentTime = 0.0f; RecalculateBoneTransform(); }
	void TogglePlayPause() { m_IsPlaying = !m_IsPlaying; }

	bool IsPlaying() const { return m_IsPlaying; }

	// Scrub to an explicit time (e.g. from a slider), in ticks
	void SeekTime(float ticks)
	{
		if (!m_CurrentAnimation) return;
		float duration = m_CurrentAnimation->GetDuration();
		m_CurrentTime = duration > 0.0f ? fmod(ticks, duration) : 0.0f;
		if (m_CurrentTime < 0.0f) m_CurrentTime += duration; // handle negative wrap
		RecalculateBoneTransform();
	}

	// Scrub by normalized 0..1 (convenient for an ImGui slider)
	void SeekNormalized(float t01)
	{
		if (!m_CurrentAnimation) return;
		SeekTime(t01 * m_CurrentAnimation->GetDuration());
	}


	float GetCurrentTime() const { return m_CurrentTime; }
	float GetNormalizedTime() const
	{
		if (!m_CurrentAnimation || m_CurrentAnimation->GetDuration() <= 0.0f) return 0.0f;
		return m_CurrentTime / m_CurrentAnimation->GetDuration();
	}
	float GetDuration() const { return m_CurrentAnimation ? m_CurrentAnimation->GetDuration() : 0.0f; }

	float& GetPlaybackSpeedRef() { return m_PlaybackSpeed; } // handy for ImGui::SliderFloat binding



	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:
	// Recompute the pose immediately after a seek/stop, without waiting for the next UpdateAnimation call.
	// Useful so scrubbing feels responsive even while paused.
	void RecalculateBoneTransform()
	{
		if (m_CurrentAnimation)
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
	bool m_IsPlaying;
	float m_PlaybackSpeed;

};
