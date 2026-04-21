---
pagetitle: Release Notes for MX25LM51245G OCTOSPI Component Driver
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for
# <mark>MX25LM51245G Component Driver </mark>
Copyright &copy; 2017-2025 STMicroelectronics\

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

This driver provides a set of functions offered by MX25LM51245G OCTOSPI component

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section14" checked aria-hidden="true">
<label for="collapse-section14" aria-hidden="true">__V3.0.3 / 21-Feb-2025__</label>
<div>

## Main Changes

- fix: update SIOO field as optional 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section13" aria-hidden="true">
<label for="collapse-section13" aria-hidden="true">__V3.0.2 / 29-June-2023__</label>
<div>

## Main Changes

- Update license 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section12" aria-hidden="true">
<label for="collapse-section12" aria-hidden="true">__V3.0.1 / 18-October-2022__</label>
<div>

## Main Changes

- Fix spelling errors 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section11" aria-hidden="true">
<label for="collapse-section11" aria-hidden="true">__V3.0.0 / 21-June-2022__</label>
<div>

## Main Changes

- Drivers updated to rely on HAL XSPI instead of HAL OSPI 

## Dependencies

-	This version is to be used with HAL library containing the XSPI drivers.

## Backward Compatibility

-	This version breaks the compatibility with previous versions

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section10" aria-hidden="true">
<label for="collapse-section10" aria-hidden="true">__V2.0.8 / 01-February-2022__</label>
<div>

## Main Changes

- Update license 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section9" aria-hidden="true">
<label for="collapse-section9" aria-hidden="true">__V2.0.7 / 24-December-2021__</label>
<div>

## Main Changes

- Align command structure with HAL 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">__V2.0.6 / 04-May-2021__</label>
<div>

## Main Changes

- Fix spelling warnings

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">__V2.0.5 / 31-December-2020__</label>
<div>

## Main Changes

- Check Misra-C 2012 coding rules compliance
- Formatting update

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6"  aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">__V2.0.4 / 24-Juin-2020__</label>
<div>

## Main Changes

- Codespell miss-spelling errors correction.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5"  aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">__V2.0.3 / 27-March-2020__</label>
<div>

## Main Changes

- Update mx25lm51245g.c so that, based on MSEL bit definition in OCTOSPI_CR register, resort to *s_command.FlashSelect* instead of *s_command.FlashId*.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">__V2.0.2 / 05-February-2020__</label>
<div>

## Main Changes

- Minor change to fix misra warnings

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">__V2.0.1 / 15-November-2019__</label>
<div>

## Main Changes

- Minor change on mx25lm51245g.h driver

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V2.0.0 / 30-October-2019__</label>
<div>

## Main Changes

- First Official Release of MX25LM51245G octal SPI Component drivers in line with STM32Cube BSP drivers development guidelines (UM2298) 
- The component drivers are composed of
  - component core drivers files: mx25lm51245g.h/.c
  - component configuration file mx25lm51245g_conf_template.h file

## Backward Compatibility

- This version breaks the compatibility with previous versions

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.0.0 / 07-August-2017__</label>
<div>

## Main Changes

- First official release of **MX25LM51245G** octal SPI driver 

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [[www.st.com](http://www.st.com/STM32)]{style="font-color: blue;"}
</footer>
