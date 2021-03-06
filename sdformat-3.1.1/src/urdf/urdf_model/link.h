/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Wim Meeussen */

#ifndef URDF_INTERFACE_LINK_H
#define URDF_INTERFACE_LINK_H
#include "visible.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "joint.h"
#include "color.h"

namespace urdf{

class SDFORMAT_HIDDEN Geometry
{
public:
  enum {SPHERE, BOX, CYLINDER, MESH} type;

  virtual ~Geometry(void)
  {
  }  
};

class SDFORMAT_HIDDEN Sphere : public Geometry
{
public:
  Sphere() { this->clear(); type = SPHERE; };
  double radius;

  void clear()
  {
    radius = 0;
  };
};

class SDFORMAT_HIDDEN Box : public Geometry
{
public:
  Box() { this->clear(); type = BOX; };
  Vector3 dim;

  void clear()
  {
    this->dim.clear();
  };
};

class SDFORMAT_HIDDEN Cylinder : public Geometry
{
public:
  Cylinder() { this->clear(); type = CYLINDER; };
  double length;
  double radius;

  void clear()
  {
    length = 0;
    radius = 0;
  };
};

class SDFORMAT_HIDDEN Mesh : public Geometry
{
public:
  Mesh() { this->clear(); type = MESH; };
  std::string filename;
  Vector3 scale;

  void clear()
  {
    filename.clear();
    // default scale
    scale.x = 1;
    scale.y = 1;
    scale.z = 1;
  };
};

class SDFORMAT_HIDDEN Material
{
public:
  Material() { this->clear(); };
  std::string name;
  std::string texture_filename;
  Color color;

  void clear()
  {
    color.clear();
    texture_filename.clear();
    name.clear();
  };
};

class SDFORMAT_HIDDEN Inertial
{
public:
  Inertial() { this->clear(); };
  Pose origin;
  double mass;
  double ixx,ixy,ixz,iyy,iyz,izz;

  void clear()
  {
    origin.clear();
    mass = 0;
    ixx = ixy = ixz = iyy = iyz = izz = 0;
  };
};

class SDFORMAT_HIDDEN Visual
{
public:
  Visual() { this->clear(); };
  Pose origin;
  boost::shared_ptr<Geometry> geometry;

  std::string material_name;
  boost::shared_ptr<Material> material;

  void clear()
  {
    origin.clear();
    material_name.clear();
    material.reset();
    geometry.reset();
    name.clear();
  };

  std::string name;
};

class SDFORMAT_HIDDEN Collision
{
public:
  Collision() { this->clear(); };
  Pose origin;
  boost::shared_ptr<Geometry> geometry;

  void clear()
  {
    origin.clear();
    geometry.reset();
    name.clear();
  };

  std::string name;

};


class SDFORMAT_HIDDEN Link
{
public:
  Link() { this->clear(); };

  std::string name;

  /// inertial element
  boost::shared_ptr<Inertial> inertial;

  /// visual element
  boost::shared_ptr<Visual> visual;

  /// collision element
  boost::shared_ptr<Collision> collision;

  /// if more than one collision element is specified, all collision elements are placed in this array (the collision member points to the first element of the array)
  std::vector<boost::shared_ptr<Collision> > collision_array;

  /// if more than one visual element is specified, all visual elements are placed in this array (the visual member points to the first element of the array)
  std::vector<boost::shared_ptr<Visual> > visual_array;

  /// Parent Joint element
  ///   explicitly stating "parent" because we want directional-ness for tree structure
  ///   every link can have one parent
  boost::shared_ptr<Joint> parent_joint;

  std::vector<boost::shared_ptr<Joint> > child_joints;
  std::vector<boost::shared_ptr<Link> > child_links;

  boost::shared_ptr<Link> getParent() const
  {return parent_link_.lock();};

  void setParent(const boost::shared_ptr<Link> &parent)
  { parent_link_ = parent; }
  
  void clear()
  {
    this->name.clear();
    this->inertial.reset();
    this->visual.reset();
    this->collision.reset();
    this->parent_joint.reset();
    this->child_joints.clear();
    this->child_links.clear();
    this->collision_array.clear();
    this->visual_array.clear();
  };

private:
  boost::weak_ptr<Link> parent_link_;

};




}

#endif
