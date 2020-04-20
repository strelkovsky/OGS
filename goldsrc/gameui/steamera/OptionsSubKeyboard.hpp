/*
 * This file is part of OGS Engine
 * Copyright (C) 2018, 2020 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

#include "vgui2/controls/EditablePanel.h"

class COptionsSubKeyboard : public vgui2::EditablePanel
{
public:
	COptionsSubKeyboard(vgui2::Panel *apParent);
	~COptionsSubKeyboard();
	
	void OnResetData() override;
	void OnApplyChanges() override;
	void OnKeyCodePressed(vgui2::KeyCode anCode) override;
	void OnThink() override;
private:
	void Finish();
	
	void CreateKeyBindingList();
};