#include "Parameters.h"
#include "Parameter.h"
#include "StiToolkit.h"
#include <stdexcept>

Parameters::Parameters()
  : Named("noName"),
    Described("noDesc")
{}

Parameters::Parameters(const string & name, const string & description)
  : Named(name),
    Described(description)
{}

Parameters::Parameters(const Parameters & parameter)
{
  ParameterConstIterator iter;
  for (iter=parameter.begin();
       iter!=parameter.end();
       iter++)
    {
      parameterVector.push_back(*iter);
    }
}

Parameters::~Parameters()
{
}

const Parameters & Parameters::operator=(const Parameters & parameter)
{
  ParameterConstIterator iter;
  for (iter=parameter.begin();
       iter!=parameter.end();
       iter++)
    {
      parameterVector.push_back(*iter);
    } 
  return *this;
}

void Parameters::add(const string & name, 
		     const string & description,
		     double value,
		     int type)
{
  Parameter * par = StiToolkit::instance()->getParameterFactory()->getObject();
  par->set(name,description,value,type);
  add(par);
}

void Parameters::add(Parameter * parameter)
{
  parameterVector.push_back(parameter);
}

ParameterIterator Parameters::begin() 
{
  return parameterVector.begin();
}

ParameterIterator Parameters::end() 
{
  return parameterVector.end();
}

ParameterConstIterator Parameters::begin() const
{
  return parameterVector.begin();
}

ParameterConstIterator Parameters::end() const
{
  return parameterVector.end();
}

Parameter * Parameters::getParameter(const string name)
{
  ParameterIterator iter;
  for (iter=parameterVector.begin();
       iter!=parameterVector.end();
       iter++)
    {
      if ((*iter)->isName(name))
	return *iter;
    } 
  throw runtime_error("Parameters::getParameter(name) - ERROR - Requested name not found");
}

