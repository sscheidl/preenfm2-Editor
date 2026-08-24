# tAUREON Experimental AI Development Notice

This repository is part of an experimental tAUREON initiative investigating
the practical benefits and limits of artificial intelligence in the
development of audio applications, synthesizer editors and synthesizer
firmware.

## Development approach

The project intentionally includes experimental **AI-assisted “vibe coding.”**
Substantial design, implementation, modernization, debugging, documentation and
code-review work may be produced through human-directed collaboration with
AI coding systems, principally **OpenAI Codex** and **Anthropic Claude Code**.

The human maintainer defines the goals, selects and integrates changes, performs
or commissions reviews, and tests the resulting software with real hosts and
hardware. AI-generated suggestions are not assumed to be correct merely because
they compile or pass an automated check.

## Research purpose

The work is intended to explore questions including:

- where AI assistance accelerates modernization of older audio code;
- how reliably AI can reason about real-time audio, MIDI protocols, plug-in
  hosts and embedded synthesizer firmware;
- which defects AI-assisted review can find or introduce;
- which safeguards, independent reviews and hardware tests remain necessary;
- how maintainable an AI-assisted project remains over multiple releases.

Both successful results and discovered limitations are relevant outcomes of
this experiment.

## Experimental status and responsibility

Unless a release explicitly states otherwise, tAUREON builds should be treated
as experimental software. Compilation, automated tests and initial smoke tests
do not prove correct behaviour in every DAW, MIDI configuration or hardware
state. Users should preserve backups, avoid irreplaceable preset banks during
early testing, and independently verify operations that write to hardware.

The repository remains subject to its existing licence and upstream copyright
notices. AI assistance does not replace or diminish the credit owed to the
original developers and other human contributors. This is an independent
community experiment; it is not an official product of, or endorsed by,
OpenAI or Anthropic.

This notice is intended as the common disclosure for experimental tAUREON
software projects and may be reused in their repositories.
