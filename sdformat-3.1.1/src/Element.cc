/*
 * Copyright 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "sdf/Assert.hh"
#include "sdf/Element.hh"

using namespace sdf;

/////////////////////////////////////////////////
Element::Element()
  : dataPtr(new ElementPrivate)
{
  this->dataPtr->copyChildren = false;
}

/////////////////////////////////////////////////
Element::~Element()
{
  this->dataPtr->parent.reset();
  for (Param_V::iterator iter = this->dataPtr->attributes.begin();
      iter != this->dataPtr->attributes.end(); ++iter)
  {
    (*iter).reset();
  }
  this->dataPtr->attributes.clear();

  for (ElementPtr_V::iterator iter = this->dataPtr->elements.begin();
      iter != this->dataPtr->elements.end(); ++iter)
  {
    (*iter).reset();
  }

  for (ElementPtr_V::iterator iter = this->dataPtr->elementDescriptions.begin();
      iter != this->dataPtr->elementDescriptions.end(); ++iter)
  {
    (*iter).reset();
  }
  this->dataPtr->elements.clear();
  this->dataPtr->elementDescriptions.clear();

  this->dataPtr->value.reset();

  delete this->dataPtr;
  this->dataPtr = NULL;

  // this->Reset();
}

/////////////////////////////////////////////////
ElementPtr Element::GetParent() const
{
  return this->dataPtr->parent;
}

/////////////////////////////////////////////////
void Element::SetParent(const ElementPtr _parent)
{
  this->dataPtr->parent = _parent;
}

/////////////////////////////////////////////////
void Element::SetName(const std::string &_name)
{
  this->dataPtr->name = _name;
}

/////////////////////////////////////////////////
const std::string &Element::GetName() const
{
  return this->dataPtr->name;
}

/////////////////////////////////////////////////
void Element::SetRequired(const std::string &_req)
{
  this->dataPtr->required = _req;
}

/////////////////////////////////////////////////
const std::string &Element::GetRequired() const
{
  return this->dataPtr->required;
}

/////////////////////////////////////////////////
void Element::SetCopyChildren(bool _value)
{
  this->dataPtr->copyChildren = _value;
}

/////////////////////////////////////////////////
bool Element::GetCopyChildren() const
{
  return this->dataPtr->copyChildren;
}

/////////////////////////////////////////////////
void Element::AddValue(const std::string &_type,
    const std::string &_defaultValue, bool _required,
    const std::string &_description)
{
  this->dataPtr->value = this->CreateParam(this->dataPtr->name,
      _type, _defaultValue, _required, _description);
}

/////////////////////////////////////////////////
boost::shared_ptr<Param> Element::CreateParam(const std::string &_key,
    const std::string &_type, const std::string &_defaultValue, bool _required,
    const std::string &_description)
{
  return boost::shared_ptr<Param>(
        new Param(_key, _type, _defaultValue, _required, _description));
}

/////////////////////////////////////////////////
void Element::AddAttribute(const std::string &_key, const std::string &_type,
    const std::string &_defaultValue, bool _required,
    const std::string &_description)
{
  this->dataPtr->attributes.push_back(
      this->CreateParam(_key, _type, _defaultValue, _required, _description));
}

/////////////////////////////////////////////////
ElementPtr Element::Clone() const
{
  ElementPtr clone(new Element);
  clone->dataPtr->description = this->dataPtr->description;
  clone->dataPtr->name = this->dataPtr->name;
  clone->dataPtr->required = this->dataPtr->required;
  // clone->parent = this->dataPtr->parent;
  clone->dataPtr->copyChildren = this->dataPtr->copyChildren;
  clone->dataPtr->includeFilename = this->dataPtr->includeFilename;

  Param_V::const_iterator aiter;
  for (aiter = this->dataPtr->attributes.begin();
       aiter != this->dataPtr->attributes.end(); ++aiter)
  {
    clone->dataPtr->attributes.push_back((*aiter)->Clone());
  }

  ElementPtr_V::const_iterator eiter;
  for (eiter = this->dataPtr->elementDescriptions.begin();
      eiter != this->dataPtr->elementDescriptions.end(); ++eiter)
  {
    clone->dataPtr->elementDescriptions.push_back((*eiter)->Clone());
  }

  for (eiter = this->dataPtr->elements.begin();
       eiter != this->dataPtr->elements.end(); ++eiter)
  {
    clone->dataPtr->elements.push_back((*eiter)->Clone());
    clone->dataPtr->elements.back()->dataPtr->parent = clone;
  }

  if (this->dataPtr->value)
    clone->dataPtr->value = this->dataPtr->value->Clone();

  return clone;
}

/////////////////////////////////////////////////
void Element::Copy(const ElementPtr _elem)
{
  this->dataPtr->name = _elem->GetName();
  this->dataPtr->description = _elem->GetDescription();
  this->dataPtr->required = _elem->GetRequired();
  this->dataPtr->copyChildren = _elem->GetCopyChildren();
  this->dataPtr->includeFilename = _elem->dataPtr->includeFilename;

  for (Param_V::iterator iter = _elem->dataPtr->attributes.begin();
       iter != _elem->dataPtr->attributes.end(); ++iter)
  {
    if (!this->HasAttribute((*iter)->GetKey()))
      this->dataPtr->attributes.push_back((*iter)->Clone());
    ParamPtr param = this->GetAttribute((*iter)->GetKey());
    (*param) = (**iter);
  }

  if (_elem->GetValue())
  {
    if (!this->dataPtr->value)
      this->dataPtr->value = _elem->GetValue()->Clone();
    else
      *(this->dataPtr->value) = *(_elem->GetValue());
  }

  this->dataPtr->elementDescriptions.clear();
  for (ElementPtr_V::const_iterator iter =
       _elem->dataPtr->elementDescriptions.begin();
       iter != _elem->dataPtr->elementDescriptions.end(); ++iter)
  {
    this->dataPtr->elementDescriptions.push_back((*iter)->Clone());
  }

  this->dataPtr->elements.clear();
  for (ElementPtr_V::iterator iter = _elem->dataPtr->elements.begin();
       iter != _elem->dataPtr->elements.end(); ++iter)
  {
    ElementPtr elem = (*iter)->Clone();
    elem->Copy(*iter);
    elem->dataPtr->parent = shared_from_this();
    this->dataPtr->elements.push_back(elem);
  }
}

/////////////////////////////////////////////////
void Element::PrintDescription(const std::string &_prefix)
{
  std::cout << _prefix << "<element name ='" << this->dataPtr->name
            << "' required ='" << this->dataPtr->required << "'>\n";

  std::cout << _prefix << "  <description>" << this->dataPtr->description
            << "</description>\n";

  Param_V::iterator aiter;
  for (aiter = this->dataPtr->attributes.begin();
      aiter != this->dataPtr->attributes.end(); ++aiter)
  {
    std::cout << _prefix << "  <attribute name ='"
              << (*aiter)->GetKey() << "' type ='" << (*aiter)->GetTypeName()
              << "' default ='" << (*aiter)->GetDefaultAsString()
              << "' required ='" << (*aiter)->GetRequired() << "'>\n";
    std::cout << _prefix << "    <description>" << (*aiter)->GetDescription()
              << "</description>\n";
    std::cout << _prefix << "  </attribute>\n";
  }

  if (this->GetCopyChildren())
    std::cout << _prefix << "  <element copy_data ='true' required ='*'/>\n";

  ElementPtr_V::iterator eiter;
  for (eiter = this->dataPtr->elementDescriptions.begin();
      eiter != this->dataPtr->elementDescriptions.end(); ++eiter)
  {
    (*eiter)->PrintDescription(_prefix + "  ");
  }

  std::cout << _prefix << "</element>\n";
}

/////////////////////////////////////////////////
void Element::PrintDocRightPane(std::string &_html, int _spacing, int &_index)
{
  std::ostringstream stream;
  ElementPtr_V::iterator eiter;

  int start = _index++;

  std::string childHTML;
  for (eiter = this->dataPtr->elementDescriptions.begin();
      eiter != this->dataPtr->elementDescriptions.end(); ++eiter)
  {
    (*eiter)->PrintDocRightPane(childHTML, _spacing + 4, _index);
  }

  stream << "<a name=\"" << this->dataPtr->name << start
         << "\">&lt" << this->dataPtr->name << "&gt</a>";

  stream << "<div style='padding-left:" << _spacing << "px;'>\n";

  stream << "<div style='background-color: #ffffff'>\n";

  stream << "<font style='font-weight:bold'>Description: </font>";
  if (!this->dataPtr->description.empty())
    stream << this->dataPtr->description << "<br>\n";
  else
    stream << "none<br>\n";

  stream << "<font style='font-weight:bold'>Required: </font>"
         << this->dataPtr->required << "&nbsp;&nbsp;&nbsp;\n";

  stream << "<font style='font-weight:bold'>Type: </font>";
  if (this->dataPtr->value)
  {
    stream << this->dataPtr->value->GetTypeName()
           << "&nbsp;&nbsp;&nbsp;\n"
           << "<font style='font-weight:bold'>Default: </font>"
           << this->dataPtr->value->GetDefaultAsString() << '\n';
  }
  else
    stream << "n/a\n";

  stream << "</div>";

  if (this->dataPtr->attributes.size() > 0)
  {
    stream << "<div style='background-color: #dedede; padding-left:10px; "
           << "display:inline-block;'>\n";
    stream << "<font style='font-weight:bold'>Attributes</font><br>";

    Param_V::iterator aiter;
    for (aiter = this->dataPtr->attributes.begin();
        aiter != this->dataPtr->attributes.end(); ++aiter)
    {
      stream << "<div style='display: inline-block;padding-bottom: 4px;'>\n";

      stream << "<div style='float:left; width: 80px;'>\n";
      stream << "<font style='font-style: italic;'>" << (*aiter)->GetKey()
        << "</font>: ";
      stream << "</div>\n";

      stream << "<div style='float:left; padding-left: 4px; width: 300px;'>\n";

      if (!(*aiter)->GetDescription().empty())
          stream << (*aiter)->GetDescription() << "<br>\n";
      else
          stream << "no description<br>\n";

      stream << "<font style='font-weight:bold'>Type: </font>"
             << (*aiter)->GetTypeName() << "&nbsp;&nbsp;&nbsp;"
        << "<font style='font-weight:bold'>Default: </font>"
        << (*aiter)->GetDefaultAsString() << "<br>";
      stream << "</div>\n";

      stream << "</div>\n";
    }
    stream << "</div>\n";
    stream << "<br>\n";
  }

  _html += stream.str();
  _html += childHTML;
  _html += "</div>\n";
}

/////////////////////////////////////////////////
void Element::PrintDocLeftPane(std::string &_html, int _spacing, int &_index)
{
  std::ostringstream stream;
  ElementPtr_V::iterator eiter;

  int start = _index++;

  std::string childHTML;
  for (eiter = this->dataPtr->elementDescriptions.begin();
      eiter != this->dataPtr->elementDescriptions.end(); ++eiter)
  {
    (*eiter)->PrintDocLeftPane(childHTML, _spacing + 4, _index);
  }

  stream << "<a id='" << start << "' onclick='highlight(" << start
         << ");' href=\"#" << this->dataPtr->name << start
         << "\">&lt" << this->dataPtr->name << "&gt</a>";

  stream << "<div style='padding-left:" << _spacing << "px;'>\n";

  _html += stream.str();
  _html += childHTML;
  _html += "</div>\n";
}

/////////////////////////////////////////////////
void Element::PrintValues(std::string _prefix)
{
  std::cout << _prefix << "<" << this->dataPtr->name;

  Param_V::iterator aiter;
  for (aiter = this->dataPtr->attributes.begin();
       aiter != this->dataPtr->attributes.end(); ++aiter)
  {
    std::cout << " " << (*aiter)->GetKey() << "='"
      << (*aiter)->GetAsString() << "'";
  }

  if (this->dataPtr->elements.size() > 0)
  {
    std::cout << ">\n";
    ElementPtr_V::iterator eiter;
    for (eiter = this->dataPtr->elements.begin();
        eiter != this->dataPtr->elements.end(); ++eiter)
    {
      (*eiter)->PrintValues(_prefix + "  ");
    }
    std::cout << _prefix << "</" << this->dataPtr->name << ">\n";
  }
  else
  {
    if (this->dataPtr->value)
    {
      std::cout << ">" << this->dataPtr->value->GetAsString()
        << "</" << this->dataPtr->name << ">\n";
    }
    else
    {
      std::cout << "/>\n";
    }
  }
}

/////////////////////////////////////////////////
std::string Element::ToString(const std::string &_prefix) const
{
  std::ostringstream out;
  this->ToString(_prefix, out);
  return out.str();
}

/////////////////////////////////////////////////
void Element::ToString(const std::string &_prefix,
                       std::ostringstream &_out) const
{
  if (this->dataPtr->includeFilename.empty())
  {
    _out << _prefix << "<" << this->dataPtr->name;

    Param_V::const_iterator aiter;
    for (aiter = this->dataPtr->attributes.begin();
        aiter != this->dataPtr->attributes.end(); ++aiter)
    {
      _out << " " << (*aiter)->GetKey() << "='"
           << (*aiter)->GetAsString() << "'";
    }

    if (this->dataPtr->elements.size() > 0)
    {
      _out << ">\n";
      ElementPtr_V::const_iterator eiter;
      for (eiter = this->dataPtr->elements.begin();
          eiter != this->dataPtr->elements.end(); ++eiter)
      {
        (*eiter)->ToString(_prefix + "  ", _out);
      }
      _out << _prefix << "</" << this->dataPtr->name << ">\n";
    }
    else
    {
      if (this->dataPtr->value)
      {
        _out << ">" << this->dataPtr->value->GetAsString()
             << "</" << this->dataPtr->name << ">\n";
      }
      else
      {
        _out << "/>\n";
      }
    }
  }
  else
  {
    _out << _prefix << "<include filename='"
         << this->dataPtr->includeFilename << "'/>\n";
  }
}

/////////////////////////////////////////////////
bool Element::HasAttribute(const std::string &_key)
{
  return this->GetAttribute(_key) != NULL;
}

/////////////////////////////////////////////////
bool Element::GetAttributeSet(const std::string &_key)
{
  bool result = false;
  ParamPtr p = this->GetAttribute(_key);
  if (p)
    result = p->GetSet();

  return result;
}

/////////////////////////////////////////////////
ParamPtr Element::GetAttribute(const std::string &_key)
{
  Param_V::const_iterator iter;
  for (iter = this->dataPtr->attributes.begin();
      iter != this->dataPtr->attributes.end(); ++iter)
  {
    if ((*iter)->GetKey() == _key)
      return (*iter);
  }
  return ParamPtr();
}

/////////////////////////////////////////////////
size_t Element::GetAttributeCount() const
{
  return this->dataPtr->attributes.size();
}

/////////////////////////////////////////////////
ParamPtr Element::GetAttribute(unsigned int _index) const
{
  ParamPtr result;
  if (_index < this->dataPtr->attributes.size())
    result = this->dataPtr->attributes[_index];

  return result;
}

/////////////////////////////////////////////////
size_t Element::GetElementDescriptionCount() const
{
  return this->dataPtr->elementDescriptions.size();
}

/////////////////////////////////////////////////
ElementPtr Element::GetElementDescription(unsigned int _index) const
{
  ElementPtr result;
  if (_index < this->dataPtr->elementDescriptions.size())
    result = this->dataPtr->elementDescriptions[_index];
  return result;
}

/////////////////////////////////////////////////
ElementPtr Element::GetElementDescription(const std::string &_key) const
{
  ElementPtr_V::const_iterator iter;
  for (iter = this->dataPtr->elementDescriptions.begin();
       iter != this->dataPtr->elementDescriptions.end(); ++iter)
  {
    if ((*iter)->GetName() == _key)
      return (*iter);
  }

  return ElementPtr();
}

/////////////////////////////////////////////////
ParamPtr Element::GetValue()
{
  return this->dataPtr->value;
}

/////////////////////////////////////////////////
bool Element::HasElement(const std::string &_name) const
{
  ElementPtr_V::const_iterator iter;
  for (iter = this->dataPtr->elements.begin();
       iter != this->dataPtr->elements.end(); ++iter)
  {
    if ((*iter)->GetName() == _name)
      return true;
  }

  return false;
}

/////////////////////////////////////////////////
ElementPtr Element::GetElementImpl(const std::string &_name) const
{
  ElementPtr_V::const_iterator iter;
  for (iter = this->dataPtr->elements.begin();
       iter != this->dataPtr->elements.end(); ++iter)
  {
    if ((*iter)->GetName() == _name)
      return (*iter);
  }

  // gzdbg << "Unable to find element [" << _name << "] return empty\n";
  return ElementPtr();
}

/////////////////////////////////////////////////
ElementPtr Element::GetFirstElement() const
{
  if (this->dataPtr->elements.empty())
    return ElementPtr();
  else
    return this->dataPtr->elements.front();
}

/////////////////////////////////////////////////
ElementPtr Element::GetNextElement(const std::string &_name) const
{
  if (this->dataPtr->parent)
  {
    ElementPtr_V::const_iterator iter;
    iter = std::find(this->dataPtr->parent->dataPtr->elements.begin(),
        this->dataPtr->parent->dataPtr->elements.end(), shared_from_this());

    if (iter == this->dataPtr->parent->dataPtr->elements.end())
    {
      return ElementPtr();
    }

    ++iter;
    if (iter == this->dataPtr->parent->dataPtr->elements.end())
      return ElementPtr();
    else if (_name.empty())
      return *(iter);
    else
    {
      for (; iter != this->dataPtr->parent->dataPtr->elements.end(); ++iter)
      {
        if ((*iter)->GetName() == _name)
          return (*iter);
      }
    }
  }

  return ElementPtr();
}

/////////////////////////////////////////////////
ElementPtr Element::GetElement(const std::string &_name)
{
  if (this->HasElement(_name))
    return this->GetElementImpl(_name);
  else
    return this->AddElement(_name);
}

/////////////////////////////////////////////////
void Element::InsertElement(ElementPtr _elem)
{
  this->dataPtr->elements.push_back(_elem);
}

/////////////////////////////////////////////////
bool Element::HasElementDescription(const std::string &_name)
{
  bool result = false;
  ElementPtr_V::const_iterator iter;
  for (iter = this->dataPtr->elementDescriptions.begin();
       iter != this->dataPtr->elementDescriptions.end(); ++iter)
  {
    if ((*iter)->dataPtr->name == _name)
    {
      result = true;
      break;
    }
  }

  return result;
}

/////////////////////////////////////////////////
ElementPtr Element::AddElement(const std::string &_name)
{
  ElementPtr_V::const_iterator iter, iter2;
  for (iter = this->dataPtr->elementDescriptions.begin();
      iter != this->dataPtr->elementDescriptions.end(); ++iter)
  {
    if ((*iter)->dataPtr->name == _name)
    {
      ElementPtr elem = (*iter)->Clone();
      elem->SetParent(shared_from_this());
      this->dataPtr->elements.push_back(elem);

      // Add all child elements.
      for (iter2 = elem->dataPtr->elementDescriptions.begin();
           iter2 != elem->dataPtr->elementDescriptions.end(); ++iter2)
      {
        // Add only required child element
        if ((*iter2)->GetRequired() == "1")
        {
          elem->AddElement((*iter2)->dataPtr->name);
        }
      }

      return this->dataPtr->elements.back();
    }
  }
  sdferr << "Missing element description for [" << _name << "]\n";
  return ElementPtr();
}

/////////////////////////////////////////////////
void Element::ClearElements()
{
  for (sdf::ElementPtr_V::iterator iter = this->dataPtr->elements.begin();
      iter != this->dataPtr->elements.end(); ++iter)
  {
    (*iter)->ClearElements();
  }

  this->dataPtr->elements.clear();
}

/////////////////////////////////////////////////
void Element::Update()
{
  for (sdf::Param_V::iterator iter = this->dataPtr->attributes.begin();
      iter != this->dataPtr->attributes.end(); ++iter)
  {
    (*iter)->Update();
  }

  for (sdf::ElementPtr_V::iterator iter = this->dataPtr->elements.begin();
      iter != this->dataPtr->elements.end(); ++iter)
  {
    (*iter)->Update();
  }

  if (this->dataPtr->value)
    this->dataPtr->value->Update();
}

/////////////////////////////////////////////////
void Element::Reset()
{
  for (ElementPtr_V::iterator iter = this->dataPtr->elements.begin();
      iter != this->dataPtr->elements.end(); ++iter)
  {
    if (*iter)
      (*iter)->Reset();
    (*iter).reset();
  }

  for (ElementPtr_V::iterator iter = this->dataPtr->elementDescriptions.begin();
      iter != this->dataPtr->elementDescriptions.end(); ++iter)
  {
    if (*iter)
      (*iter)->Reset();
    (*iter).reset();
  }
  this->dataPtr->elements.clear();
  this->dataPtr->elementDescriptions.clear();

  this->dataPtr->value.reset();

  this->dataPtr->parent.reset();
}

/////////////////////////////////////////////////
void Element::AddElementDescription(ElementPtr _elem)
{
  this->dataPtr->elementDescriptions.push_back(_elem);
}

/////////////////////////////////////////////////
void Element::SetInclude(const std::string &_filename)
{
  this->dataPtr->includeFilename = _filename;
}

/////////////////////////////////////////////////
std::string Element::GetInclude() const
{
  return this->dataPtr->includeFilename;
}

/////////////////////////////////////////////////
std::string Element::GetDescription() const
{
  return this->dataPtr->description;
}

/////////////////////////////////////////////////
void Element::SetDescription(const std::string &_desc)
{
  this->dataPtr->description = _desc;
}

/////////////////////////////////////////////////
void Element::RemoveFromParent()
{
  if (this->dataPtr->parent)
  {
    ElementPtr_V::iterator iter;
    iter = std::find(this->dataPtr->parent->dataPtr->elements.begin(),
        this->dataPtr->parent->dataPtr->elements.end(), shared_from_this());

    if (iter != this->dataPtr->parent->dataPtr->elements.end())
    {
      this->dataPtr->parent->dataPtr->elements.erase(iter);
      this->dataPtr->parent.reset();
    }
  }
}

/////////////////////////////////////////////////
void Element::RemoveChild(ElementPtr _child)
{
  SDF_ASSERT(_child, "Cannot remove a NULL child pointer");

  ElementPtr_V::iterator iter;
  iter = std::find(this->dataPtr->elements.begin(),
                   this->dataPtr->elements.end(), _child);

  if (iter != this->dataPtr->elements.end())
  {
    _child->SetParent(ElementPtr());
    this->dataPtr->elements.erase(iter);
  }
}

/////////////////////////////////////////////////
boost::any Element::GetAny(const std::string &_key)
{
  boost::any result;
  if (_key.empty() && this->dataPtr->value)
  {
    if (!this->dataPtr->value->GetAny(result))
    {
      sdferr << "Couldn't get element [" << this->GetName()
             << "] as boost::any\n";
    }
  }
  else if (!_key.empty())
  {
    ParamPtr param = this->GetAttribute(_key);
    if (param)
    {
      if (!this->GetAttribute(_key)->GetAny(result))
        sdferr << "Couldn't get attribute [" << _key << "] as boost::any\n";
    }
    else if (this->HasElement(_key))
      result = this->GetElementImpl(_key)->GetAny();
    else if (this->HasElementDescription(_key))
      result = this->GetElementDescription(_key)->GetAny();
    else
      sdferr << "Unable to find value for key [" << _key << "]\n";
  }
  return result;
}
