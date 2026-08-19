/*
 * Copyright 2017 Xavier Hosxe
 *
 * Author: Xavier Hosxe (xavier <dot> hosxe
 *                      (at) g m a i l <dot> com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "MidifiedFloatParameter.h"

void MidifiedFloatParameter::setValue(float newValue) {
	float tmpValue = range.convertFrom0to1(newValue);
	if (value != tmpValue) {
		value = tmpValue;
		jassert(audioProcessor != nullptr);
		if (audioProcessor == nullptr) {
			return;
		}
		audioProcessor->hostParameterChanged(getParameterIndex());
	}
}

void MidifiedFloatParameter::setValueFromNrpn(int nrpnValue) {
	float newValue = getValueFromNrpn(nrpnValue);
	value = newValue;
}

void MidifiedFloatParameter::setRealValue(float newValue) {
	if (value != newValue) {
		value = newValue;
		jassert(audioProcessor != nullptr);
		if (audioProcessor == nullptr) {
			return;
		}
		audioProcessor->onParameterUpdated(this);
	}
}


void MidifiedFloatParameter::setPfmBankValue(float pfmValue) {
	// We update value but as we're in a fake processor
	// we dont need to update any other value 
	
	// For float we don't change anything (float is when multiplier > 1)
	// For int we add min value
	if (valueMultiplier == 1.0f) {
		value = pfmValue + pfm2MinValue - bias;
	}
	else {
		value = pfmValue;
	}
}


float MidifiedFloatParameter::getPfmBankValue() {
	if (valueMultiplier == 1.0f) {
		return value - pfm2MinValue + bias;
	}
	else {
		return value;
	}
}

