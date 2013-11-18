/**
 * @file nodice/config.cpp
 * @brief Implemntation of the nodice/config module.
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
#include "nodice/config.h"

#include <cstring>
#include <iostream>


namespace
{
	/**
	 * Tries to extract an option argument.
	 * @param[in]  a1
	 * @param[in]  a2
	 * @param[out] index
	 */
	static const char*
	getarg(const char* a1, const char* a2, int& index)
	{
		if (std::strlen(a1) > 0)
		{
			return a1;
		}
		if (a2)
		{
			++index;
			return a2;
		}
		return NULL;
	}
}


/**
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Vector of command-line argument strings.
 *
 * Parses the command line arguments and sets variaous configurable items
 * appropriately.
 *
 * This contains a local reimplementation of getopt(3) because not all target
 * platforms support the POSIX API.
 */
NoDice::Config::
Config(int argc, char* argv[])
: m_isDirty(false)
, m_isDebugMode(false)
, m_isFullscreen(false)
, m_isSmallWindow(false)
, m_screenWidth(640)
, m_screenHeight(480)
, m_boardSize(8)
{
	for (int i = 0; i < argc; ++i)
	{
		if (*argv[i] == '-')
		{
			char c = *(argv[i] + 1);
			switch (c)
			{
				case 'd':
				{
					m_isDebugMode = true;
					break;
				}

				case 'f':
				{
					m_isFullscreen = true;
					break;
				}

				case 'w':
				{
					m_isSmallWindow = true;
					break;
				}

				case 't':
				{
					const char* opt = getarg(argv[i]+2, (i < argc) ? argv[i+1] : NULL, i);
					if (opt == NULL)
					{
						std::cerr << "error parsing arg -t\n";
						break;
					}
					std::cerr << "arg t opt '" << opt << "'\n";
					break;
				}
			}
		}
	}
}


NoDice::Config::
~Config()
{
}


bool NoDice::Config::
isDebugMode() const
{
	return m_isDebugMode;
}


bool NoDice::Config::
isFullscreen() const
{
	return m_isFullscreen;
}


bool NoDice::Config::
isSmallWindow() const
{
	return m_isSmallWindow;
}


int NoDice::Config::
screenWidth() const
{
	return m_screenWidth;
}


void NoDice::Config::
setScreenWidth(int w)
{
	if (m_screenWidth != w)
	{
		m_screenWidth = w;
		setDirty();
	}
}


int NoDice::Config::
screenHeight() const
{
	return m_screenHeight;
}


void NoDice::Config::
setScreenHeight(int h)
{
	if (m_screenHeight != h)
	{
		m_screenHeight = h;
		setDirty();
	}
}


int NoDice::Config::
boardSize() const
{ return m_boardSize; }


void NoDice::Config::
setBoardSize(int size)
{
	if (m_boardSize != size)
	{
		m_boardSize = size;
		setDirty();
	}
}


void NoDice::Config::
setDirty()
{
	m_isDirty = true;
}
