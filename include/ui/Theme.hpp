#pragma once

#include <QString>

// Central place for the app's dark-theme Qt style sheets, so the same colours
// and widget styling are not copy-pasted across every dialog and page.
namespace Theme
{
// Colour palette (kept here for reference / reuse).
inline constexpr auto WINDOW     = "#242F32";
inline constexpr auto SURFACE    = "#1B272A";
inline constexpr auto SURFACE_HI = "#2F3D41";
inline constexpr auto TEXT       = "#E1E1E2";
inline constexpr auto ACCENT     = "#337BFF";

// Full style sheet for the app's modal dialogs (inputs, combos, list views,
// scroll bars and buttons). %1 is substituted with the drop-down arrow icon.
inline QString dialogStyle(const QString &downIcon)
{
	return QString(R"(
			QDialog {
				background-color: #242F32;
				color: #E1E1E2;
				font-family: "Helvetica Neue";
			}
			QLabel {
				color: #E1E1E2;
			}
			QLineEdit, QComboBox, QPlainTextEdit {
				background-color: #1B272A;
				color: #E1E1E2;
				border: 1px solid #444;
				border-radius: 4px;
				padding: 4px;
				min-height: 28px;
				padding-left: 6px;
			}
			QComboBox::drop-down {
				subcontrol-origin: padding;
				subcontrol-position: top right;
				width: 25px;
				border-left: 1px solid #444;
				background-color: #2F3D41;
			}
			QComboBox::down-arrow {
				image: url(%1);
				width: 12px;
				height: 12px;
			}
			QListView {
				background-color: #1B272A;
				color: #E1E1E2;
			}
			QListView::item:selected {
				background-color: #2F3D41;
				color: #E1E1E2;
			}
			QScrollBar:vertical {
				background: transparent;
				width: 8px;
				margin: 3px 0;
				border-radius: 4px;
			}
			QScrollBar::handle:vertical {
				background: #1B272A;
				min-height: 20px;
				border-radius: 4px;
			}
			QScrollBar::add-line:vertical,
			QScrollBar::sub-line:vertical {
				height: 0;
			}
			QScrollBar::add-page:vertical,
			QScrollBar::sub-page:vertical {
				background: none;
			}
			QPushButton {
				background-color: #1B272A;
				color: #E1E1E2;
				border: none;
				padding: 6px 12px;
				border-radius: 2px;
				min-width: 80px;
			}
			QPushButton:hover {
				background-color: #2F3D41;
			}
			QPushButton:disabled {
				color: gray;
			}
		)")
	    .arg(downIcon);
}

// Style sheet for the filter combo boxes on the History / Stats pages.
// %1 is substituted with the drop-down arrow icon.
inline QString comboBoxStyle(const QString &downIcon)
{
	return QString(R"(
			QComboBox {
				background-color: #1B272A;
				color: #E1E1E2;
				border: 1px solid #444;
				border-radius: 4px;
				padding-left: 6px;
				min-height: 28px;
				min-width: 90px;
			}
			QComboBox::drop-down {
				subcontrol-origin: padding;
				subcontrol-position: top right;
				width: 25px;
				border-left: 1px solid #444;
				background-color: #2F3D41;
			}
			QComboBox::down-arrow {
				image: url(%1);
				width: 12px;
				height: 12px;
			}
		)")
	    .arg(downIcon);
}
} // namespace Theme
