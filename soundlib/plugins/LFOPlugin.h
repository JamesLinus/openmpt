/*
* LFOPlugin.h
* -----------
* Purpose: Plugin for automating other plugins' parameters
* Notes  : (currently none)
* Authors: OpenMPT Devs
* The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
*/


#pragma once

#ifndef NO_PLUGINS

#include "PlugInterface.h"
#include "../../common/mptRandom.h"

OPENMPT_NAMESPACE_BEGIN

//=================================
class LFOPlugin : public IMixPlugin
//=================================
{
	friend class LFOPluginEditor;

protected:
	enum Parameters
	{
		kAmplitude = 0,
		kOffset,
		kFrequency,
		kTempoSync,
		kWaveform,
		kPolarity,
		kBypassed,
		kCurrentPhase,
		kLFONumParameters
	};

	enum LFOWaveform
	{
		kSine = 0,
		kTriangle,
		kSaw,
		kSquare,
		kSHNoise,
		kSmoothNoise,
		kNumWaveforms
	};

	std::vector<mpt::byte> m_chunkData;

	// LFO parameters
	float m_amplitude, m_offset, m_frequency;
	LFOWaveform m_waveForm;
	PlugParamIndex m_outputParam;
	bool m_tempoSync, m_polarity, m_bypassed, m_outputToCC;

	// LFO state
	double m_computedFrequency;
	double m_phase, m_increment;
	double m_random, m_nextRandom;
	double m_tempo;

	mpt::fast_prng m_PRNG;

#ifdef MODPLUG_TRACKER
	static const int WM_PARAM_UDPATE = WM_USER + 500;
#endif

public:
	static IMixPlugin* Create(VSTPluginLib &factory, CSoundFile &sndFile, SNDMIXPLUGIN *mixStruct);
	LFOPlugin(VSTPluginLib &factory, CSoundFile &sndFile, SNDMIXPLUGIN *mixStruct);

	void Release() override { delete this; }
	int32 GetUID() const override { int32 id; memcpy(&id, "LFO ", 4); return id; }
	int32 GetVersion() const override { return 0; }
	void Idle() override { }
	uint32 GetLatency() const override { return 0; }

	void Process(float *pOutL, float *pOutR, uint32 numFrames) override;

	float RenderSilence(uint32) override { return 0.0f; }

	int32 GetNumPrograms() const override { return 0; }
	int32 GetCurrentProgram() override { return 0; }
	void SetCurrentProgram(int32) override { }

	PlugParamIndex GetNumParameters() const override { return kLFONumParameters; }
	PlugParamValue GetParameter(PlugParamIndex index) override;
	void SetParameter(PlugParamIndex index, PlugParamValue value) override;

	void Resume() override;
	void Suspend() override { m_isResumed = false; }
	void PositionChanged() override;

	bool IsInstrument() const override { return false; }
	bool CanRecieveMidiEvents() override { return false; }
	bool ShouldProcessSilence() override { return true; }

	void MidiCommand(uint8, uint8, uint16, uint16, uint16, CHANNELINDEX) { SetParameter(kCurrentPhase, 0); }

#ifdef MODPLUG_TRACKER
	CString GetDefaultEffectName() override { return _T("LFO"); }

	CString GetParamName(PlugParamIndex param) override;
	CString GetParamLabel(PlugParamIndex) override;
	CString GetParamDisplay(PlugParamIndex param) override;

	CString GetCurrentProgramName() override { return CString(); }
	void SetCurrentProgramName(const CString &) override { }
	CString GetProgramName(int32) override { return CString(); }

	bool HasEditor() const override { return true; }
protected:
	CAbstractVstEditor *OpenEditor() override;
#endif

public:
	int GetNumInputChannels() const override { return 2; }
	int GetNumOutputChannels() const override { return 2; }

	bool ProgramsAreChunks() const override { return true; }
	// Save parameters for storing them in a module file
	void SaveAllParameters() override;
	// Restore parameters from module file
	void RestoreAllParameters(int32 program) override;
	ChunkData GetChunk(bool) override;
	void SetChunk(const ChunkData &chunk, bool) override;

protected:
	void NextRandom();
	void RecalculateFrequency();
	void RecalculateIncrement();
	PLUGINDEX GetOutputPlugin() const;

public:
	static LFOWaveform ParamToWaveform(float param) { return static_cast<LFOWaveform>(Util::Round<int>(param * 32.0f)); }
	static float WaveformToParam(LFOWaveform waveform) { return waveform / 32.0f; }
};

OPENMPT_NAMESPACE_END

#endif // NO_PLUGINS
