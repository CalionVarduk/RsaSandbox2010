#pragma once

#include "LabelButton.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;

	public ref class SecondaryForm : public Form
	{
		public:
			property bool IsShown {
				public: bool get() { return _isShown; }
				private: void set(bool value) { _isShown = value; }
			}

			property bool IsModal {
				public: bool get() { return _isModal; }
				public: void set(bool value) { _isModal = value; }
			}

			SecondaryForm(LabelButton^ controlButton) : Form()
			{
				InitializeComponent();
				_controlButton = controlButton;
				if(_controlButton != nullptr) {
					_controlButton->ActivatedOnClick = true;
					_controlButton->OnClickDeactivatable = false;
					_controlButton->addMouseDownEvent(gcnew MouseEventHandler(this, &SecondaryForm::Event_ControlButtonClicked));
				}
				_isShown = false;
				_isModal = false;
			}

			void display()
			{
				if(_isShown) {
					System::Media::SystemSounds::Beep->Play();
					Focus();
				}
				else {
					_isShown = true;
					if(_isModal) ShowDialog();
					else Show();
					Refresh();
				}
			}

			bool closeIfShown()
			{
				if(_controlButton->Activated) {
					Close();
					return true;
				}
				return false;
			}

		protected:
			~SecondaryForm()
			{
				if (components)
					delete components;
			}

		private:
			LabelButton^ _controlButton;
			bool _isShown;
			bool _isModal;
			System::ComponentModel::Container^ components;

			void Event_ControlButtonClicked(Object^ sender, MouseEventArgs^ e)
			{ display(); }

			void Event_FormClosing(Object^ sender, FormClosingEventArgs^ e)
			{
				e->Cancel = true;
				if(_controlButton != nullptr)
					_controlButton->deactivate();
				_isShown = false;
				Hide();
			}

		#pragma region Windows Form Designer generated code
			void InitializeComponent(void)
			{
				this->SuspendLayout();
				this->components = gcnew System::ComponentModel::Container();
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
				this->MaximizeBox = false;
				this->MinimizeBox = false;
				this->TopMost = true;
				this->Name = L"SecondaryForm";
				this->Text = L"SecondaryForm";
				this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SecondaryForm::Event_FormClosing);
				this->ResumeLayout(false);

			}
		#pragma endregion
	};
}