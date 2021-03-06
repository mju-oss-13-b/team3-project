/**
 * @file nodice/videocontextsdl.cpp
 * @brief Implemntation of the nodice/videocontextsdl module.
 *
 * Copyright 2009, 2010 Stephen M. Webb  <stephen.webb@bregmasoft.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of Version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "nodice/videocontextsdl.h"

#include <iostream>
#include "nodice/config.h"
#include <SDL/SDL.h>


NoDice::VideoContextSDL::
VideoContextSDL(Config& config)
{
	if (0 != ::SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		std::cerr << "*** ERRROR in SDL_InitSubSystem(SDL_INIT_VIDEO): "
		          << ::SDL_GetError() << "\n";
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
	if (!videoInfo)
	{
		std::cerr << "*** ERRROR in SDL_GetVideoInfo(): "
		          << ::SDL_GetError() << "\n";
		exit(1);
	}
	if (config.isDebugMode())
		std::cerr << "Video Info:\n"
		          << "  current_w=" << videoInfo->current_w << "\n"
		          << "  current_h=" << videoInfo->current_h << "\n"
		          << "  vfmt->BitsPerPixel="
		          << static_cast<int>(videoInfo->vfmt->BitsPerPixel) << "\n";

	SDL_Rect** modes = SDL_ListModes(videoInfo->vfmt, SDL_OPENGL|SDL_FULLSCREEN);
//	SDL_Rect** modes = SDL_ListModes(videoInfo->vfmt, SDL_OPENGL);
	if (modes == NULL)
	{
		std::cerr << "*** ERRROR no supported video modes available.\n";
		exit(1);
	}

	int width = config.isFullscreen() ? videoInfo->current_w : config.screenWidth();
	int height = config.isFullscreen() ? videoInfo->current_h : config.screenHeight();
	if (modes == (SDL_Rect **)-1)
	{
		std::cerr << "==smw> all video resolutions available.\n";
	}
	else
	{
		if (config.isDebugMode())
		{
			std::cerr << "Available Modes:\n";
			for (int i=0; modes[i]; ++i)
			{
				std::cerr << "  " << modes[i]->w << "x" << modes[i]->h << "\n";
			}
		}

		bool mode_found = false;
		for (int i=0; modes[i]; ++i)
		{
			if (modes[i]->w == width && modes[i]->h == height)
			{
				mode_found = true;
				break;
			}
		}
		if (!mode_found && modes[0])
		{
			if (config.isDebugMode())
				std::cerr << ".. mode " << width << "x" << height
				          << "not found, falling back to default.\n";
			if (config.isSmallWindow())
			{
				width = 640;
				height = 480;
			}
			else
			{
				width = modes[0]->w;
				height = modes[0]->h;
			}
		}
	}
	if (config.isDebugMode())
		std::cerr << ".. choosing window size " << width
				<< "x" << height
				<< "x" << static_cast<int>(videoInfo->vfmt->BitsPerPixel) << ".\n";

	Uint32 videoFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;
	if (config.isFullscreen())
	{
		if (config.isDebugMode())
			std::cerr << ".. choosing full-screen mode\n";
		videoFlags |= SDL_FULLSCREEN;
	}
	else if (config.isDebugMode())
	{
		std::cerr << ".. choosing windowed mode\n";
	}

	SDL_Surface* surface = SDL_SetVideoMode(width, height, videoInfo->vfmt->BitsPerPixel, videoFlags);
	if (!surface)
	{
		std::cerr << "*** ERRROR in SDL_SetVideoMode(): " << ::SDL_GetError() << "\n";
		exit(1);
	}

	videoInfo = SDL_GetVideoInfo();
	config.setScreenWidth(videoInfo->current_w);
	config.setScreenHeight(videoInfo->current_h);
}

void NoDice::VideoContextSDL::swapBuffers()
{
	SDL_GL_SwapBuffers();
}


