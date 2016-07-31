#pragma once

#include "LabelButtonList.h"
#include "DynamicControlCollection.h"
#include "ColorFactory.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class SimpleBox abstract
	{
		public:
			static property Color BackColor {
				public: Color get() { return _box->BackColor; }
				public: void set(Color value) { _box->BackColor = value; }
			}

			static property Color ForeColor {
				public: Color get() { return _info->ForeColor; }
				public: void set(Color value) { _info->ForeColor = value; }
			}

			static property Color ButtonBackColorNormal {
				public: Color get() { return _buttons[0]->BackColorNormal; }
				public: void set(Color value) { _buttons->BackColorNormal = value; }
			}

			static property Color ButtonForeColorNormal {
				public: Color get() { return _buttons[0]->ForeColorNormal; }
				public: void set(Color value) { _buttons->ForeColorNormal = value; }
			}

			static property Color ButtonBackColorMouseOver {
				public: Color get() { return _buttons[0]->BackColorMouseOver; }
				public: void set(Color value) { _buttons->BackColorMouseOver = value; }
			}

			static property Color ButtonForeColorMouseOver {
				public: Color get() { return _buttons[0]->ForeColorMouseOver; }
				public: void set(Color value) { _buttons->ForeColorMouseOver = value; }
			}

			static property Font^ InfoFont {
				public: Font^ get() { return _info->Font; }
				public: void set(Font^ value) { _info->Font = value; }
			}

			static property Font^ ButtonsFont {
				public: Font^ get() { return _buttons[0]->Font; }
				public: void set(Font^ value)
						{
							_buttons->Font = value;
							Int32 height = _buttons[0]->PreferredSize.Height + 8;
							_buttons[0]->Height = height;
							_buttons[1]->Height = height;
							ConfirmText = ConfirmText;
							CancelText = CancelText;
						}
			}

			static property String^ ConfirmText {
				public: String^ get() { return _buttons[0]->Text; }
				public: void set(String^ value)
						{
							_buttons[0]->Text = value;
							Int32 width = _buttons[0]->PreferredSize.Width + 20;
							if(width < _buttons[1]->Width) width = _buttons[1]->Width;
							_buttons[0]->Width = width;
							_buttons[1]->Width = width;
							_buttons->performLayout();
						}
			}

			static property String^ CancelText {
				public: String^ get() { return _buttons[1]->Text; }
				public: void set(String^ value)
						{
							_buttons[1]->Text = value;
							Int32 width = _buttons[1]->PreferredSize.Width + 20;
							if(width < _buttons[0]->Width) width = _buttons[0]->Width;
							_buttons[0]->Width = width;
							_buttons[1]->Width = width;
							_buttons->performLayout();
						}
			}

			static property bool IsShown {
				public: bool get() { return _box->Visible; }
			}

			static property LabelButtonList^ ButtonCollection {
				public: LabelButtonList^ get() { return _buttons; }
			}

			static property Icon^ BoxIcon {
				public: Icon^ get() { return _box->Icon; }
				public: void set(Icon^ value) { _box->Icon = value; }
			}

			static SimpleBox()
			{
				_box->Name = "SimpleBox";
				_box->FormBorderStyle = FormBorderStyle::FixedToolWindow;
				_box->MinimizeBox = false;
				_box->MaximizeBox = false;
				_box->TopMost = true;

				_info->Name = "SimpleBoxInfo";
				_info->Font = gcnew Font("Microsoft Sans Serif", 10, FontStyle::Bold);
				_info->AutoSize = true;
				_info->BackColor = Color::Transparent;
				_info->Top = 12;
				_box->Controls->Add(_info);

				_buttons->IsListHorizontal = true;
				_buttons->Spacing = Point(10, 0);
				_buttons->addButton("SimpleBoxConfirm", "");
				_buttons->addButton("SimpleBoxCancel", "");

				_buttons->ActivatedOnClick = true;
				_buttons->OnClickDeactivatable = true;
				_buttons->ColorableActivation = false;
				_buttons->ColorableMouseOver = true;

				_buttons->BackColorNormal = Color::SteelBlue;
				_buttons->ForeColorNormal = Color::White;
				_buttons->BackColorMouseOver = ColorFactory::Scale(Color::SteelBlue, 1.15f);
				_buttons->ForeColorMouseOver = Color::White;
				ButtonsFont = _info->Font;
				ConfirmText = "Confirm";
				CancelText = "Cancel";

				_buttons[0]->addMouseDownEvent(gcnew MouseEventHandler(&Event_ButtonClicked));
				_buttons[1]->addMouseDownEvent(gcnew MouseEventHandler(&Event_ButtonClicked));
				_box->FormClosing += gcnew FormClosingEventHandler(&Event_BoxClosing);
				_box->Load += gcnew EventHandler(&Event_BoxLoad);
			}

			static void display(String^ message)
			{
				_buttons->Visible = false;
				_info->Text = message;
				_info->Left = 12;
				_box->ClientSize = Size(_info->PreferredWidth + 24, _info->Top + _info->PreferredHeight + 12);
				_box->ShowDialog();
				_info->Text = "";
			}

			static void display(array<String^>^ message)
			{
				display(_buildMessage(message));
			}

			static void display(String^ message, String^ title)
			{
				_box->Text = title;
				display(message);
				_box->Text = "";
			}

			static void display(array<String^>^ message, String^ title)
			{
				display(_buildMessage(message), title);
			}

			static DialogResult displayWithResult(String^ message)
			{
				_buttons->Visible = true;
				_buttons[0]->deactivate();
				_buttons[1]->deactivate();

				_info->Text = message;

				Int32 dWidth = _buttons->Width - _info->PreferredWidth;
				if(dWidth < 0) {
					_info->Left = 12;
					_buttons->Location = Point(12 - (dWidth >> 1), _info->Top + _info->PreferredHeight + 12);
					_box->ClientSize = Size(_info->PreferredWidth + 24, _buttons->Bottom + 12);
				}
				else {
					_info->Left = 12 + (dWidth >> 1);
					_buttons->Location = Point(12, _info->Top + _info->PreferredHeight + 12);
					_box->ClientSize = Size(_buttons->Right + 12, _buttons->Bottom + 12);
				}

				_box->ShowDialog();
				_info->Text = "";

				if(_buttons[0]->Activated) return DialogResult::OK;
				if(_buttons[1]->Activated) return DialogResult::Cancel;
				return DialogResult::Abort;
			}

			static DialogResult displayWithResult(array<String^>^ message)
			{
				return displayWithResult(_buildMessage(message));
			}

			static DialogResult displayWithResult(String^ message, String^ title)
			{
				_box->Text = title;
				DialogResult result = displayWithResult(message);
				_box->Text = "";
				return result;
			}

			static DialogResult displayWithResult(array<String^>^ message, String^ title)
			{
				return displayWithResult(_buildMessage(message), title);
			}

			static void display(DynamicControlCollection^ controls)
			{
				_buttons->Visible = false;
				controls->Parent = _box;
				controls->Location = Point(12, 12);
				_box->ClientSize = Size(controls->Right + 12, controls->Bottom + 12);
				_box->ShowDialog();
				controls->Parent = nullptr;
			}

			static void display(DynamicControlCollection^ controls, String^ title)
			{
				_box->Text = title;
				display(controls);
				_box->Text = "";
			}

			static void close()
			{
				_box->Close();
			}

		private:
			static Form^ _box = gcnew Form();
			static Label^ _info = gcnew Label();
			static LabelButtonList^ _buttons = gcnew LabelButtonList(_box);

			static String^ _buildMessage(array<String^>^ message)
			{
				StringBuilder^ sb = gcnew StringBuilder(100);
				Int32 mLength = message->Length - 1;
				for(Int32 i = 0; i < mLength; ++i) sb->Append(message[i] + "\r\n");
				return sb->Append(message[mLength])->ToString();
			}

			static void Event_BoxClosing(Object^ sender, FormClosingEventArgs^ e)
			{
				e->Cancel = true;
				_box->Hide();
			}

			static void Event_BoxLoad(Object^ sender, EventArgs^ e)
			{
				_box->SetDesktopLocation(Cursor::Position.X - (_box->Width >> 1), Cursor::Position.Y - (_box->Height >> 1));
			}

			static void Event_ButtonClicked(Object^ sender, MouseEventArgs^ e)
			{
				_box->Close();
			}
	};
}