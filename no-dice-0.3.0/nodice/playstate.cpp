/**
 * @file nodice/playstate.cpp
 * @brief Implemntation of the nodice/playstate module.
 *
 * Copyright 2010, 2011 Stephen M. Webb  <stephen.webb@bregmasoft.ca>
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
#include "nodice_config.h"
#include "nodice/playstate.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include "nodice/colour.h"
#include "nodice/font.h"
#include "nodice/object.h"
#include "nodice/shape.h"
#include "nodice/video.h"

#define SCORE_FONT "FreeSans"


namespace
{
  static const GLfloat near =  0.0f;
  static const GLfloat far  = 10.0f;
  static const NoDice::Vector3f board_scale(1.0f/12.0f, 1.0f/12.0f, 1.0f/12.0f);
  static const NoDice::Vector3f board_pos(-0.3f, -0.5f, -1.0f);
  static const NoDice::Vector4f lightAmbient(0.2f, 0.2f, 0.2f, 1.0f);
  static const NoDice::Vector4f lightDiffuse(0.8f, 0.8f, 0.8f, 1.0f);
  static const NoDice::Vector4f lightPosition(2.0f, 2.0f, 3.0f, 0.0f);
  static const NoDice::Vector3f lightDirection(-2.0f, -2.0f, -3.0f);
  static const int mouseMoveThreshold = 20;
} // anonymous namespace


NoDice::PlayState::
PlayState(Config& config)
: GameState(config)
, m_state(state_idle)
, m_gameboard(config)
, m_scoreFont(getFont(SCORE_FONT, config.screenHeight() / 18))
, m_mouseIsDown(false)
, m_multiplier(0)
, m_score(0)

{
  m_timer = 210;
  // generate unproject matrix
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w = m_config.screenWidth();
  int h = m_config.screenHeight();
  float right = 1.0f;
  float top   = 1.0f;
  if (h < w)
    top = float(h) / float(w);
  else
    right = float(w) / float(h);
#ifdef HAVE_OPENGL_ES
  glFrustumf(-right, right, -top, top, near, far);
#else
  glOrtho(-right, right, -top, top, near, far);
#endif
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(board_pos.x, board_pos.y, board_pos.z);
  glScalef(board_scale.x, board_scale.y, board_scale.z);
  Matrix4f projection;
  glGetFloatv(GL_PROJECTION_MATRIX, projection.array);
  Matrix4f modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview.array);
  (projection * modelview).getInverse(m_unproject);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}


NoDice::PlayState::
~PlayState()
{
}


void NoDice::PlayState::
pointerMove(int x, int y, int dx, int dy)
{
  if (m_mouseIsDown && m_state != state_swapping)
  {
    Vector2i d = m_mouseDownPos - Vector2i(x, y);
    if (d.lengthSquared() < mouseMoveThreshold * mouseMoveThreshold)
      return;

    Vector2i pos2 = m_selectedPos;
    if (std::abs(dx) > std::abs(dy))
    {
      if (dx < 0 && m_selectedPos.x > 0)
          --pos2.x;
      else if (m_selectedPos.x < m_config.boardSize()-1)
          ++pos2.x;
      else
        return;
    }
    else
    {
      if (dy > 0 && m_selectedPos.y > 0)
          --pos2.y;
      else if (m_selectedPos.y < m_config.boardSize()-1)
          ++pos2.y;
      else
        return;
    }
    m_state = state_swapping;
    m_gameboard.startSwap(m_selectedPos, pos2);
  }
}


void NoDice::PlayState::
pointerClick(int x, int y, PointerAction action)
{
  if (m_timer == 0){
     return ;
  } 
  if (action == pointerUp)
  {
    m_mouseIsDown = false;
    m_mouseDownPos.set(0, 0);
  }
  else if (action == pointerDown)
  {
    m_mouseDownPos.set(x, y);
    float win_width = float(m_config.screenWidth()) / 2.0f;
    float win_height = float(m_config.screenHeight()) / 2.0f;
    float unit_x = float(x - win_width) / win_width;
    float unit_y = -float(y - win_height) / win_height;

    Vector4f ray(unit_x, unit_y, 0.0f, 1.0f);
    Vector4f beam = m_unproject * ray;
    m_selectedPos.x = int(beam.x / 2.0f + 0.50f);
    m_selectedPos.y = int(beam.y / 2.0f + 0.50f);
    if (m_selectedPos.x >= m_config.boardSize() || m_selectedPos.x < 0)
      return;
    if (m_selectedPos.y >= m_config.boardSize() || m_selectedPos.y < 0)
      return;

    ObjectPtr obj = m_gameboard.at(m_selectedPos.x, m_selectedPos.y);
    obj->setHighlight(true);

    m_mouseIsDown = true;
  }
}


void NoDice::PlayState::
calculateScore(const ObjectBrace& matches)
{
  for (auto it = matches.begin(); it != matches.end(); ++it)
  {
    int match_score = m_multiplier;
    std::ostringstream ostr;
    //ostr << it->size() << it->at(0)->type();
    if (m_multiplier)
    {
    //  ostr << "+" << m_multiplier;
    }
    std::cerr << ostr.str() << "(";
    for (auto obj = it->begin(); obj != it->end(); ++obj)
    {
      int score = (*obj)->score();
      std::cerr << " " << score;
      match_score += score;
    }
    std::cerr << " ) total=" << match_score << "\n";
    m_score += match_score;
    m_winMessages.push_back(ostr.str());
  }
  m_state = state_replacing;
  ++m_multiplier;
}


void NoDice::PlayState::
update(App& app NODICE_UNUSED)
{
  m_gameboard.update();
  if(m_timer == 0){
    
     return ;
  }
  switch (m_state)
  {
    case state_swapping:
    {
      m_timer--;
      if (!m_gameboard.isSwapping())
      {
        ObjectBrace matches(m_gameboard.findWins());
        if (matches.size())
        {
          calculateScore(matches);
        }
        else
        {
          m_gameboard.unSwap();
          m_state = state_unswapping;
          m_multiplier = 0;
        }
      }
      break;
    }

    case state_unswapping:
    {
      m_timer--;
      if (!m_gameboard.isSwapping())
      {
        m_state = state_idle; // temp. for now
      }
      break;
    }

    case state_replacing:
    {
      if (!m_gameboard.isReplacing())
      {
        ObjectBrace matches(m_gameboard.findWins());
        if (matches.size() > 0)
        {
          calculateScore(matches);
        }
        else
        {
          std::cerr << __FUNCTION__ << " has no wins\n";
          m_state = state_idle;
          m_multiplier = 0;
          m_winMessages.clear();
        }
      }
      break;
    }

                default:
                  break;
  }
}


void NoDice::PlayState::
draw(Video& video NODICE_UNUSED) 
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // Adjust projection to take aspect ratio into account.
  int w = m_config.screenWidth();
  int h = m_config.screenHeight();
  float right = 1.0f;
  float top   = 1.0f;
  if (h < w)
    top = float(h) / float(w);
  else
    right = float(w) / float(h);
#ifdef HAVE_OPENGL_ES
  glFrustumf(-right, right, -top, top, near, far);
#else
  glOrtho(-right, right, -top, top, near, far);
#endif
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white.rgba);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient.rgba);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse.rgba);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white.rgba);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition.xyzw);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection.xyz);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 1.2f);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20.0f);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(board_pos.x, board_pos.y, board_pos.z);
  glScalef(board_scale.x, board_scale.y, board_scale.z);
  m_gameboard.draw();
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  float y = 300.0f;
  std::ostringstream ostr;
  std::ostringstream ostr2;
  ostr << std::setw(5) << std::setfill('0') << m_score ;
  ostr2 << m_timer/21 ;
  glColor4fv(white.rgba);
  m_scoreFont.print(10.0f, y, 1.0f, ostr.str());
  m_scoreFont.print(10.0f, 250.0f, 1.0f, ostr2.str());
  if(m_timer == 0){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_scoreFont.print(300.0f,300.0f, 1.0f, ostr.str());
  }
  for (auto it = m_winMessages.begin(); it != m_winMessages.end(); ++it)
  {

  glDisable(GL_DEPTH_TEST);
    y -= 30;
    m_scoreFont.print(10.0f, y, 0.8f, *it);
  }
  check_gl_error("playstate END");
}


