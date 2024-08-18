---
layout: default
title: Patterns
nav_order: 20
parent: Vortex Engine
has_children: true
---

# Patterns
Patterns control the timing and appearance of blinks on an LED. When a pattern is used with a [colorset](colorsets.html), each blink of the pattern sequentially displays the next color in the colorset.

## Simple Pattern Usage

The basic way to use a pattern is to simply choose one via the [Pattern Select Menu](pattern_select_menu.html). With this appraoch nothing else needs to be known about patterns.

However, if you want to learn how patterns truly work or learn more about each pattern then continue reading.

## Pattern Types

All patterns are classified into a **type** and **family**, there are only **two families** of patterns: 

  - **A Multi-LED Pattern** plays on all LEDs, creating coordinated effects that _travel_ across LEDs.

  - **A Single-LED Pattern** plays on an individual LED and _does not travel_.

Within each **family** there are a variety of **types** of patterns. Examples of **types** of **single-led patterns** include: _blend_, or _strobe_.

All patterns have **parameters**, numeric values that control their appearance, such as duration of blinks or gaps. Each **type** of pattern may offer slightly different **parameters** to control.

The **Pattern Select Menu** offers a curated list of patterns to choose from, these are various **types** of patterns that have their **parameters** tuned to specific values.

Two completely different looking patterns in the **Pattern Select Menu** may actually be the same **type** of pattern with slightly different **paremeters**.

## Multi Led Patterns

To learn more about Multi-Led Patterns see the [Multi-LED Patterns](multi_led_patterns.html) page.

_Note: The Duo does not support Multi-LED patterns._

## Single Led Patterns

To learn more about Single-Led Patterns see the [Single-LED Patterns](single_led_patterns.html) page.
