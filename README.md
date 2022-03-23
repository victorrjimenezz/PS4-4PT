<div id="top"></div>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/victorrjimenezz/4PT-Repository">
    <img src="sce_sys/icon0.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">4PT: PS4 APT</h3>

  <p align="center">
    An Advanced Packaging Tool for the PS4
    <br />
    <a href="https://github.com/victorrjimenezz/4PT-Repository/issues"><strong>Request Feature / Report Bug</strong></a>
    <br />
    <br />
    <a href="https://www.4pt-project.com">Website</a>
    ·
    <a href="https://github.com/victorrjimenezz/4PT-Repository">4PT Template Repository</a>
    ·
    <a href="https://github.com/victorrjimenezz/4PT-REPO-GUI">4PT Repository GUI</a>
    ·
    <a href="https://github.com/victorrjimenezz/PS4-4PT/blob/master/TUTORIAL.md">App tutorial</a>
  </p>
</div>




<!-- TABLE OF CONTENTS -->
<details open>
  <summary>Table of Contents</summary>
  <ol>
    <li>
        <a href="#ata">About The App</a>
      <ul>
        <li><a href="#ss">Screenshots</a></li>
      </ul>
    </li>
    <li><a href="#feat">Features</a></li>
    <li><a href="#usage">Usage</a></li>
    <li>
      <a href="#roadmap">Roadmap</a>
      <ul>
        <li><a href="#st">Short Term</a></li>
        <li><a href="#lt">Long Term</a></li>
      </ul>
    </li>
    <li><a href="#lang">Languages</a></li>
    <li><a href="#buildDep">Build Dependencies</a></li>
    <li><a href="#developers">Developers</a></li>
    <li><a href="#Acknowledgments">Acnkowledgments</a></li>
  </ol>
</details>

***

<div id="ata"></div>

## About The App

There are many great Homebrew Apps/Games available.

This tool was developed in order for PS4 Homebrew users to easily download PKGs without the need of using a computer.

Furthermore, 4PT allows anyone to host their own repository and provide any apps/games they would like to share.

<div id="ss"></div>


### Screenshots

<details open>
  <summary>Screenshots</summary>



<p float="left">
  <img src="screenshots/screenshot0.png" width="49%"  alt=""/>
  <img src="screenshots/screenshot1.png" width="49%"  alt=""/>
</p>
<p float="left">
  <img src="screenshots/screenshot01.png" width="49%"  alt=""/>
  <img src="screenshots/screenshot2.png" width="49%"  alt=""/>
</p>
<p float="left">
  <img src="screenshots/screenshot3.png" width="49%"  alt=""/>
  <img src="screenshots/screenshot4.png" width="49%"  alt=""/>
</p>
<p float="left">
  <img src="screenshots/screenshot41.png" width="49%"  alt=""/>
  <img src="screenshots/screenshot6.png" width="49%"  alt=""/>
</p>
</details>

***

<div id="feat"></div>

## Features

The current working app features are:

- Downloading PKGs from repositories/directly from a URL
- Ability to pause/resume a download
- Adding/Removing Repositories
- Searching apps, filtering by name, repository, and type
- Sorting apps, alphabetically, by size, or by version
- Install/Uninstall downloaded PKGs
- App has English, Hungarian, Italian, Spanish, and German support
- 4PT updates itself automatically

***

<div id="usage"></div>

## Usage

_For information on how to use the app, refer to [Tutorial](https://github.com/victorrjimenezz/PS4-4PT/blob/master/TUTORIAL.md)_

_For information on creating a repo, please refer to  [4PT-Repository](https://github.com/victorrjimenezz/4PT-Repository)_

***

<div id="roadmap"></div>

## Roadmap

<div id="st"></div>

### Version 2.75 (~2 Month)

- [ ] Text issue
- [ ] Translate Uninstall button, delete keys and pkgtypes, Unknown
- [ ] Add Http Authentication Support
- [ ] Save on external HDD
- [X] Fix crash when deleting ongoing download
- [X] Compare repo url regardless HTTP
- [ ] Filter by region (https://www.psdevwiki.com/ps4/Productcode)
- [x] Add portuguese
- [ ] Fix bug that causes the app to crash, after having changed the language and moving to the last page of the search list
- [X] Show speed and time remaining
- [X] Download pkgs while playing games (Optional in settings)
- [X] Show update available in downloads tab
- [X] Fix bug that caused update available to be shown after installing app's latest version
- [X] Improved date string generation by adding a 0 in front of single digit time units
- [X] Fix downloads tab flicker on rapidly changing selected download
- [X] Use full downloads tab

<div id="lt"></div>

### Future versions (~1/2 Year)

- [ ] Fix crash when loading slow and long repo
- [ ] Fix bug that causes the app to freeze
- [ ] Fix Hungarian automatic detection
- [ ] Implement proper App deallocation when pressing the Close Application button on the PS Menu
- [ ] Add functionality to download binaries
- [ ] get Abstract Class from packageSearch and repoPackageList
- [ ] Add a new PKG List view where only covers are displayed
- [ ] Support for more languages

See the [open issues](https://github.com/victorrjimenezz/PS4-4PT/issues) for a full list of proposed features (and known issues).

***

<div id="lang"></div>

## Languages

The app is available in:
- [X] English
- [X] Hungarian by [Damy](https://github.com/dagadtwok)
- [X] Italian by [@imgiovi](https://www.twitter.com/imgiovi)
- [X] German
- [X] Spanish

To add support for further languages, please [contact me](https://twitter.com/victorrjimenezz) or [open an issue](https://github.com/victorrjimenezz/4PT-Repository/issues)


***

<div id="buildDep"></div>

## Build Dependencies

This project is built using the Open Orbis PS4 Toolchain and the Yaml-CPP library.

* [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
* [Yaml-CPP](https://github.com/jbeder/yaml-cpp)

***

<div id="developers"></div>

## Developers

Víctor Jiménez - [@victorrjimenezz](https://twitter.com/victorrjimenezz) - [victorjimenez@4pt-project.com](mailto:victorjimenez@4pt-project.com)

<div id="Acknowledgments"></div>

***

## Acknowledgments

I would like to thank [0x199](https://twitter.com/0x199) for helping me with the SSL issue.

I would like to thank the following developers for the developing following projects, which I used to learn how to perform certain actions:

* [Remote Package Installer](https://github.com/flatz/ps4_remote_pkg_installer) by [Flat_z](https://twitter.com/flat_z) </br>
&ensp;  &ensp;  &ensp; Learned how to access PS4's native HTTP </br>
&ensp;  &ensp;  &ensp; Learned how get icon and version from pkgs  </br>
 &ensp;  &ensp;  &ensp; Learned how to install PKGs
* [PS4 Skeleton](https://github.com/Al-Azif/ps4-skeleton) by [Al Azif](https://twitter.com/_AlAzif) </br>
&ensp;  &ensp;  &ensp; Used his jailbreaking tools to gain root privileges </br>
  &ensp;  &ensp;  &ensp; Used his notifi class to send notifications
* [Vita Album](https://github.com/joel16/VITAlbum) by [joel16](https://github.com/joel16) </br>
&ensp;  &ensp;  &ensp; Used his keyboard namespace to understand how IMEDialogs work  </br>
&ensp;  &ensp;  &ensp; Used his keyboard namespace to understand how to transform UTF8 <=> UTF16 strings
* [PS4 Homebrew Store](https://github.com/LightningMods/PS4-Store) by [LightningMods_](https://twitter.com/lightningmods_) </br>
&ensp;  &ensp;  &ensp; Used his dialog method to pop dialogs </br>
&ensp;  &ensp;  &ensp; Learned how to access PS4's native HTTP </br>
&ensp;  &ensp;  &ensp; Learned how to install PKGs </br>
* [tiny-ps4-shell](https://github.com/john-tornblom/tiny-ps4-shell) by [John Törnblom](https://github.com/john-tornblom) </br>
&ensp;  &ensp;  &ensp; Used his method to copy files </br>
* [OpenOrbis NET_HTTP Sample](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/tree/master/samples/net_http) by [Damián Parrino](https://twitter.com/dparrino) </br>
&ensp;  &ensp;  &ensp; Used his setSSLCallback method to enable SSL </br>

And I would like to thank the whole OpenOrbis team for developing the awesome [PS4 Developer toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)!!
