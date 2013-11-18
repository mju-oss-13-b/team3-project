/**
 * @file nodice/videocontextsdl.h
 * @brief Public interface of the nodice/videocontextsdl module.
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
#ifndef NODICE_VIDEOCONTEXTSDL_H
#define NODICE_VIDEOCONTEXTSDL_H 1

#include "nodice/videocontext.h"


namespace NoDice
{
	class Config;


	class VideoContextSDL
	: public VideoContext
	{
	public:
		VideoContextSDL(Config& config);

		void swapBuffers();
	};

} // namespace NoDice

#endif // NODICE_VIDEOCONTEXTSDL_H
