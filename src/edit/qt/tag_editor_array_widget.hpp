// SPDX-License-Identifier: GPL-3.0-only

#ifndef INVADER__EDIT__QT__TAG_EDITOR_ARRAY_WIDGET_HPP
#define INVADER__EDIT__QT__TAG_EDITOR_ARRAY_WIDGET_HPP

#include "tag_editor_widget.hpp"

class QLineEdit;
class QComboBox;
class QVBoxLayout;

namespace Invader::EditQt {
    class TagEditorEditWidgetView;

    class TagEditorArrayWidget : public TagEditorWidget {
        Q_OBJECT

    public:
        /**
         * Instantiate an array widget
         * @param parent        parent widget
         * @param value         struct value
         * @param editor_window editor window
         */
        TagEditorArrayWidget(QWidget *parent, Parser::ParserStructValue *value, TagEditorWindow *editor_window);

    private slots:
        void perform_add();
        void perform_delete();
        void perform_duplicate();
        void perform_clear();
        void perform_shift_up();
        void perform_shift_down();

    private:
        void regenerate_widget();
        void regenerate_enum();

        TagEditorEditWidgetView *tag_view_widget = nullptr;
        QComboBox *reflexive_index;
        QVBoxLayout *vbox_layout;
    };
}

#endif
