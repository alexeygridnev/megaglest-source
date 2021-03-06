// ==============================================================
//	This file is part of Glest (www.glest.org)
//
//	Copyright (C) 2001-2008 Martiño Figueroa
//
//	You can redistribute this code and/or modify it under 
//	the terms of the GNU General Public License as published 
//	by the Free Software Foundation; either version 2 of the 
//	License, or (at your option) any later version
// ==============================================================

#include "display.h"

#include "metrics.h"
#include "command_type.h"
#include "util.h"
#include "leak_dumper.h"

using namespace Shared::Graphics;
using namespace Shared::Util;

namespace Glest{ namespace Game{

// =====================================================
// 	class Display
// =====================================================

Display::Display(){
	if(SystemFlags::getSystemSettingType(SystemFlags::debugSystem).enabled) SystemFlags::OutputDebug(SystemFlags::debugSystem,"In [%s::%s Line: %d]\n",__FILE__,__FUNCTION__,__LINE__);

	colors[0]= Vec4f(1.f, 1.f, 1.f, 0.0f);
	colors[1]= Vec4f(1.f, 0.5f, 0.5f, 0.0f);
	colors[2]= Vec4f(0.5f, 0.5f, 1.0f, 0.0f);
	colors[3]= Vec4f(0.5f, 1.0f, 0.5f, 0.0f);
	colors[4]= Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	colors[5]= Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
	colors[6]= Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
	colors[7]= Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
	colors[8]= Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

	currentColor= 0;
	clear();

	if(SystemFlags::getSystemSettingType(SystemFlags::debugSystem).enabled) SystemFlags::OutputDebug(SystemFlags::debugSystem,"In [%s::%s Line: %d]\n",__FILE__,__FUNCTION__,__LINE__);
}

void Display::calculateUpDimensions(int index) {
	if(index>maxUpIndex){
		maxUpIndex=index;
		if(maxUpIndex+1>upCellSideCount*upCellSideCount){
			upCellSideCount=upCellSideCount+1;
			upImageSize = static_cast<float>(imageSize) * static_cast<float>(cellSideCount) / static_cast<float>(upCellSideCount) + 0.9f;
		}
	}
}

Vec4f Display::getColor() const {
	if(currentColor < 0 || currentColor >= colorCount) {
		throw megaglest_runtime_error("currentColor >= colorCount");
	}
	return colors[currentColor];
}

void Display::setUpImage(int i, const Texture2D *image)
{
	if(i>=upCellCount) throw megaglest_runtime_error("i>=upCellCount in Display::setUpImage");
	upImages[i]= image;
	calculateUpDimensions(i);
}

//misc
void Display::clear(){
	downSelectedPos= invalidPos;
	for(int i=0; i<upCellCount; ++i){
		upImages[i]= NULL;
	}

	for(int i=0; i<downCellCount; ++i){
		downImages[i]= NULL;
		downImageColor[i]= Vec3f(1.0f,1.0f,1.0f);
		commandTypes[i]= NULL;
		commandClasses[i]= ccNull;
	}

	title.clear();
	text.clear();
	progressBar= -1;

	upCellSideCount=cellSideCount;
	upImageSize=imageSize;
	maxUpIndex= 0;
}
void Display::switchColor(){
	currentColor= (currentColor+1) % colorCount;
}

int Display::computeDownIndex(int x, int y) const {
	y= y-(downY-cellSideCount*imageSize);
	
	if(y>imageSize*cellSideCount || y < 0){
		return invalidPos;
	}

	int cellX= x/imageSize;
	int cellY= (y/imageSize) % cellSideCount;
	int index= (cellSideCount-cellY-1)*cellSideCount+cellX;;

	if(index<0 || index>=downCellCount || downImages[index]==NULL){
		index= invalidPos;
	}

	return index;
}

int Display::computeDownX(int index) const{
	return (index % cellSideCount) * imageSize;
}

int Display::computeDownY(int index) const{
	return Display::downY - (index/cellSideCount)*imageSize - imageSize;
}

int Display::computeUpX(int index) const{
	return (index % upCellSideCount) * upImageSize;
}

int Display::computeUpY(int index) const{
	return Metrics::getInstance().getDisplayH() - (index/upCellSideCount)*upImageSize - upImageSize;
}

void Display::saveGame(XmlNode *rootNode) const {
	std::map<string,string> mapTagReplacements;
	XmlNode *displayNode = rootNode->addChild("Display");

//	string title;
	displayNode->addAttribute("title",title, mapTagReplacements);
//	string text;
	displayNode->addAttribute("text",text, mapTagReplacements);
//	string infoText;
	displayNode->addAttribute("infoText",infoText, mapTagReplacements);
//	const Texture2D *upImages[upCellCount];
//	const Texture2D *downImages[downCellCount];
//	bool downLighted[downCellCount];
//	const CommandType *commandTypes[downCellCount];
//	CommandClass commandClasses[downCellCount];
//	int progressBar;
	displayNode->addAttribute("progressBar",intToStr(progressBar), mapTagReplacements);
//	int downSelectedPos;
	displayNode->addAttribute("downSelectedPos",intToStr(downSelectedPos), mapTagReplacements);
//	Vec4f colors[colorCount];
//	int currentColor;
	displayNode->addAttribute("currentColor",intToStr(currentColor), mapTagReplacements);
//	int upCellSideCount;
//	int upImageSize;
//	int maxUpIndex;
}

void Display::loadGame(const XmlNode *rootNode) {
	const XmlNode *displayNode = rootNode->getChild("Display");

	//	string title;
	title = displayNode->getAttribute("title")->getValue();
	//	string text;
	text = displayNode->getAttribute("text")->getValue();
	//	string infoText;
	infoText = displayNode->getAttribute("infoText")->getValue();
	//	const Texture2D *upImages[upCellCount];
	//	const Texture2D *downImages[downCellCount];
	//	bool downLighted[downCellCount];
	//	const CommandType *commandTypes[downCellCount];
	//	CommandClass commandClasses[downCellCount];
	//	int progressBar;
	progressBar = displayNode->getAttribute("progressBar")->getIntValue();
	//	int downSelectedPos;
	//displayNode->addAttribute("downSelectedPos",intToStr(downSelectedPos), mapTagReplacements);
	//	Vec4f colors[colorCount];
	//	int currentColor;
	//currentColor = displayNode->getAttribute("progressBar")->getIntValue();
	//	int upCellSideCount;
	//	int upImageSize;
	//	int maxUpIndex;
}

}}//end namespace
