#include "MainForm.h"

using namespace System;
using namespace RsaSandbox;

[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	LabelButtonScheme::Invert(SimpleBox::ButtonCollection, true, Color::SteelBlue, Color::Azure, 1.1f);
	SimpleBox::BackColor = ColorFactory::Scale(Color::AliceBlue, 0.96f);
	SimpleBox::ForeColor = ColorFactory::Scale(SimpleBox::ButtonBackColorNormal, 0.9f);
	SimpleBox::ConfirmText = "YES";
	SimpleBox::CancelText = "NO";
	SimpleBox::BoxIcon = gcnew System::Drawing::Icon(".\\cvrsa.ico");

	Application::Run(gcnew MainForm());
}