#include "DynamicControlCollection.h"

using namespace CVNetControls;

DynamicControlCollection::DynamicControlCollection()
	: _controls(gcnew List<DynamicControl^>()), _bounds(Rectangle())
{}

void DynamicControlCollection::add(DynamicControl^ control)
{
	_controls->Add(control);
}

void DynamicControlCollection::insertAt(Int32 i, DynamicControl^ control)
{
	_controls->Insert(i, control);
}

void DynamicControlCollection::removeAt(Int32 iControl)
{
	_controls->RemoveAt(iControl);
}

DynamicControl^ DynamicControlCollection::operator[] (Int32 iControl)
{ return getAt(iControl); }

DynamicControl^ DynamicControlCollection::getAt(Int32 iControl)
{ return _controls[iControl]; }

void DynamicControlCollection::clear()
{
	while(_controls->Count > 0)
		_controls->RemoveAt(0);

	_bounds.Width = 0;
	_bounds.Height = 0;
}

void DynamicControlCollection::updateBounds()
{
	if(Count > 0) {
		DynamicControl^ control = getAt(0);
		_bounds.X = control->Left;
		_bounds.Y = control->Top;
		Int32 right = control->Right;
		Int32 bottom = control->Bottom;

		for(Int32 i = 1; i < _controls->Count; ++i) {
			control = getAt(i);
			if(control->Left < _bounds.X) _bounds.X = control->Left;
			if(control->Right > right) right = control->Right;
			if(control->Top < _bounds.Y) _bounds.Y = control->Top;
			if(control->Bottom > bottom) bottom = control->Bottom;
		}

		_bounds.Width = right - _bounds.X;
		_bounds.Height = bottom - _bounds.Y;
	}
	else {
		_bounds.Width = 0;
		_bounds.Height = 0;
	}
}

void DynamicControlCollection::refresh()
{
	for(Int32 i = 0; i < _controls->Count; ++i)
		_controls[i]->refresh();
}

DynamicControl^ DynamicControlCollection::fetchControlAt(Int32 i)
{ return _controls[i]; }

array<DynamicControl^>^ DynamicControlCollection::fetchControlArray()
{ return _controls->ToArray(); }