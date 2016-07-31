#pragma once

#include "LabelButton.h"
#include "ControlTitledList.h"
#include "ColorFactory.h"

namespace CVNetControls
{
	using namespace System;

	public ref class LabelButtonScheme abstract
	{
		public:
			static void Invert(LabelButton^ button, bool clicked, Color primary, Color secondary, float colorScale)
			{
				button->ActivatedOnClick = clicked;
				button->OnClickDeactivatable = !clicked;
				button->ColorableActivation = true;
				button->ColorableMouseOver = true;

				button->BackColorActive = secondary;
				button->ForeColorActive = primary;
				button->BackColorNormal = primary;
				button->ForeColorNormal = secondary;
				button->BackColorMouseOver = ColorFactory::Scale(primary, colorScale);
				button->ForeColorMouseOver = secondary;
				button->BackColorActiveMouseOver = ColorFactory::Scale(secondary, colorScale);
				button->ForeColorActiveMouseOver = button->BackColorMouseOver;
				button->BorderColor = ColorFactory::Scale(primary, (colorScale >= 1) ? 0.85f : 1.15f);
				button->BorderThickness = 2;
			}

			static void Invert(ControlSingleList<LabelButton>^ list, bool clicked, Color primary, Color secondary, float colorScale)
			{
				for(Int32 i = 0; i < list->Count; ++i)
					Invert(list[i], clicked, primary, secondary, colorScale);
			}

			static void Invert(ControlTitledList<LabelButton>^ list, bool clicked, Color primary, Color secondary, float colorScale)
			{
				for(Int32 i = 0; i < list->Count; ++i)
					Invert(list[i], clicked, primary, secondary, colorScale);
			}
	};
}