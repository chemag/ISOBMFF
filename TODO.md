---
type: "todo"
title: "isobmff TODO"
description: "Known gaps in the isobmff fork, starting with unit test and fuzzer coverage of the registered boxes."
tags: ["isobmff", "todo", "testing", "fuzzing"]
author: "Chema Gonzalez"
generated_by: "Metamate"
date: "2026-11-10"
---

# 1. Overview

This document tracks known gaps in the isobmff fork.

| Section | Item                                             | Status       |
|---------|--------------------------------------------------|--------------|
| 2       | URN does not parse name and location             | actionable   |
| 3       | Field semantics that do not match the spec       | actionable   |
| 4       | Apply Google style to the whole tree             | actionable   |
| 5       | Resolved                                         | for reference|


# 2. URN does not parse name and location

Status: actionable, small.

`URN` (DataEntryUrnBox, "urn ") only parses the FullBox fields. Its `name`
and `location` strings are not kept, and the class has no getters for them,
so a file that references its media through a URN loses that reference.
`test/URN_unittest.cpp` documents the current behavior.


# 3. Field semantics that do not match the spec

Status: actionable, two items left.

The field names now match the specifications, and the fixed-point fields
have raw and derived getters (see "Resolved"). Left:

* `IROT::GetAngle()` returns the raw 2-bit `angle` field (0..3), which
  means angle * 90 degrees anti-clockwise. The value is right; document it
  in the header (or add a derived getter in degrees).
* `AVCC` does not parse the AVCDecoderConfigurationRecord fields that
  follow the PPSs for profiles other than 66, 77 and 88 (`chroma_format`,
  `bit_depth_luma_minus8`, `bit_depth_chroma_minus8`, and the SPS
  extension NAL units). They are missing, not misnamed.

Decided, no change: `CTTS` reads `sample_offset` as signed in both
versions, although version 0 declares it unsigned, because Apple MOV
files announce version 0 with negative offsets.


# 4. Apply Google style to the whole tree

Status: actionable, small.

The fork is too far from upstream (DigiDNA/ISOBMFF) to keep its original
formatting for easier merges. New and changed code is formatted with
`clang-format -style=google`, but much of the tree is not: around v0.19,
26 of the 192 files under src/, include/, test/ and tools/ change when
reformatted. Make a one-off commit that runs `clang-format -i
-style=google` over the whole tree (and adds a `.clang-format` with
`BasedOnStyle: Google`), with no other change, so that later diffs only
show real changes.


# 5. Resolved

* unit tests and fuzzers for every registered box. When this list was
  started, 22 of the 47 registered boxes lacked a unit test, a fuzzer, or
  both (AV01 had neither until av1C support was added in 1df0189). All 47
  now have both, `fuzz/Makefile` derives its fuzzer list from the unit
  tests instead of keeping a stale list by hand, and the `box_coverage`
  CTest (`test/check_box_coverage.cmake`) fails when a registered box has
  no unit test or fuzzer creating it. Writing the tests found a parser bug:
  infe version 2/3 did not read `item_name`, and required the optional
  `content_encoding`.
* field names that did not match the specifications (and fixed-point
  fields exposed raw). STTS called `sample_delta` `sample_offset` (the
  ctts field name). Renamed to the specification names: ImageGrid
  `rows_minus_one`/`columns_minus_one` (GetRows() returned rows minus
  one), ISPE `image_width`/`image_height` (was "display" width and
  height), HVCC `chroma_format_idc`, and ELST `edit_duration` (renamed
  from `segment_duration` in ISO/IEC 14496-12:2022). Fixed-point fields
  now have a `Get<Field>Raw()` getter with the spec's type and
  signedness, and a `Get<Field>()` getter returning a double: MVHD rate
  and volume, TKHD volume, width and height, ELST media rate, visual
  sample entry horizresolution and vertresolution, audio sample entry
  samplerate, DOPS OutputGain and HVCC avgFrameRate. Matrix values are
  doubles (16.16, and 2.30 for u, v and w); they used to read negative
  values (rotations) as large unsigned numbers.
