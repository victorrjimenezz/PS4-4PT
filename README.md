<div id="top"></div>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="sce_sys/icon0.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">PS4 APT Package Manager</h3>

  <p align="center">
    A package manager for the PS4. 
    <br />
    <a href="https://github.com/othneildrew/Best-README-Template/issues"><strong>Request Feature / Report Bug</strong></a>
    <br />
    <br />
    <a href="https://github.com/victorrjimenezz/4PT-Repository">4PT Template Repository</a>
    ·
    <a href="https://github.com/victorrjimenezz/PS4-4PT/blob/master/TUTORIAL.md">App tutorial</a>
  </p>
</div>


***

<!-- TABLE OF CONTENTS -->
<details>
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
    <li><a href="#buildDep">Build Dependencies</a></li>
    <li><a href="#developers">Developers</a></li>
    <li><a href="#Acknowledgments">Acnkowledgments</a></li>
  </ol>
</details>

***

<div id="ata"></div>

## About The App

There are many great Homebrew Apps/Games available, but there is no easy-unified way to install them.

This tool was developed in order for PS4 Homebrew users to easily download PKGs without the need of using a computer.

Furthermore, 4PT allows anyone to host their own repository and provide any apps/games they would like to share.

<div id="ss"></div>

### Screenshots
<p float="left">
  <img src="screenshots/screenshot0.png" width="225"  alt=""/>
  <img src="screenshots/screenshot1.png" width="225"  alt=""/>
</p>
<p float="left">
  <img src="screenshots/screenshot2.png" width="225"  alt=""/>
  <img src="screenshots/screenshot3.png" width="225"  alt=""/>
</p>
<p float="left">
  <img src="screenshots/screenshot4.png" width="450"  alt=""/>
</p>

***

<div id="feat"></div>

## Features

The current working app features are:

- Downloading PKGs from repositories
- Downloading PKGs directly from a URL.
- Adding Repositories
- Searching apps, filtering by name.
- Install downloaded PKGs.

***

<div id="usage"></div>

## Usage

_For information on how to use the app, refer to [Tutorial](https://github.com/victorrjimenezz/PS4-4PT/Tutorial.md)_

_For information on creating a repo, please refer to  [4PT-Repository](https://github.com/victorrjimenezz/4PT-Repository)_

***

<div id="roadmap"></div>

## Roadmap

<div id="st"></div>

### Short Term (~1.5 Months)
- [ ] Add SSL Support
- [ ] Improve Keyboard Keys
- [ ] Make a list of repoPackageList instead of initializing each time to improve efficiency
- [ ] Add option to download binaries
- [ ] Add Installed apps Update/Version management
- [ ] Add Short Description
- [ ] Add Sound Effects

<div id="lt"></div>

### Long Term (~1/2 Year)
- [ ] Add Animated GIF STBI Reading
- [ ] Improve Keyboard/Change by UI IME
- [ ] Add PS4 Version Compatibility Information
- [ ] Download Icon from PKG itself
- [ ] Compared to installed app versions
- [ ] Add Package Page with photos/videos.
- [ ] Clean .pkg From /user/app folder
- [ ] Multi-language Support

See the [open issues](https://github.com/victorrjimenezz/PS4-4PT/issues) for a full list of proposed features (and known issues).

***

<div id="buildDep"></div>

## Build Dependencies

This project is built using the Open Orbis PS4 Toolchain and the Yaml-CPP library.

* [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
* [Yaml-CPP](https://github.com/jbeder/yaml-cpp)

***

<div id="developers"></div>

## Developers

Víctor Jiménez - [@vjimenez1308](https://twitter.com/vjimenez1308)

<div id="Acknowledgments"></div>

***

## Acknowledgments

I would like to thank the following developers for the developing following projects, which I used to learn how to perform certain actions:

* [Remote Package Installer](https://github.com/flatz/ps4_remote_pkg_installer) by [Flat_z](https://twitter.com/flat_z) </br>
&ensp;  &ensp;  &ensp; Learned how access PS4's native HTTP </br>
 &ensp;  &ensp;  &ensp; Learned how to install PKGs from its source code
* [PS4 Skeleton](https://github.com/Al-Azif/ps4-skeleton) by [Al Azif](https://twitter.com/_AlAzif) </br>
&ensp;  &ensp;  &ensp; Used his jailbreaking tools to gain root privileges. </br>
  &ensp;  &ensp;  &ensp; Used his notifi class to send notifications.
* [PS4 Homebrew Store](https://github.com/LightningMods/PS4-Store) by [LightningMods_](https://twitter.com/lightningmods_) </br>
&ensp;  &ensp;  &ensp; Used his dialog method to pop dialogs. </br>
&ensp;  &ensp;  &ensp; Learned how access PS4's native HTTP. </br>
&ensp;  &ensp;  &ensp; Learned how to install PKGs from its source code. </br>
* [tiny-ps4-shell](https://github.com/john-tornblom/tiny-ps4-shell) by [John Törnblom](https://github.com/john-tornblom) </br>
&ensp;  &ensp;  &ensp; Used his method to copy files. </br>

And I would like to thank the whole OpenOrbis team for developing the awesome [PS4 Developer toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)!!
