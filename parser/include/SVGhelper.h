/*
//Name: Tyler Ykema
//ID#: 1062564
//Date: Febuary 15, 2019
//Email: tykema@uoguelph.ca
*/

#define _DEFAULT_SOURCE
#include "SVGParser.h"
#include "LinkedListAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#define LIBXML_SCHEMAS_ENABLED
#include <libxml/xmlschemastypes.h>

#define PI 3.14159265359

#define TEST 0

/********A1 Stuff*********/
void fillRect(Rectangle *, xmlAttr *);
void getData(SVGimage *, xmlNode *);
void fillCircle(Circle *, xmlAttr *);
void fillPath(Path *, xmlAttr *);
void fillAttr(Attribute *, xmlAttr *);
void fillGroup(Group *, xmlNode *);
char* RemoveDigits(char *);
void freeGetList(List *);

void getInGroup(List*, List*, char*);
void listAdd(List*, List*);

/********A2 Stuff*********/
void freeXMLStuff(xmlSchemaPtr schema, xmlDoc *doc, xmlSchemaValidCtxtPtr ctxt);
xmlDocPtr imageToxmlTree(SVGimage* image);
void getUnits(char *str, char *units);
void addAttributeToTree(xmlNodePtr node, List* list);
void addRects (xmlNodePtr root_node, List* list);
void addCircles (xmlNodePtr root_node, List* list);
void addPaths (xmlNodePtr root_node, List* list);
void addGroups (xmlNodePtr root_node, Group *group);

bool checkAttrList(List *list);
bool checkRectList(List *list);
bool checkCircList(List *list);
bool checkPathList(List *list);
bool checkGroupList(List *list);

void setRect(List *list, Attribute* newAttribute, int elemIndex);
void setCirc(List *list, Attribute *newAttribute, int elemIndex);
void setPath(List *list, Attribute *newAttribute, int elemIndex);
void setGroup(List *list, Attribute *newAttribute, int elemIndex);
void setAttr(List *list, Attribute *newAttribute);

char *nullHandler();
char *nullHandler2();

char *getTitleAndDesc(const SVGimage *img);
char *JSONcreator (const SVGimage *);
void freeStr(char *str);

bool editTitleDesc(SVGimage *img, char *title, char *desc);
