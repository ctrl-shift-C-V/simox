/**
* This file is part of Simox.
*
* Simox is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* Simox is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* @package    VirtualRobot
* @author     Nikolaus Vahrenkamp
* @copyright  2011 Nikolaus Vahrenkamp
*             GNU Lesser General Public License
*
*/
#ifndef _VirtualRobot_BaseIO_h_
#define _VirtualRobot_BaseIO_h_

#include "../VirtualRobotImportExport.h"
#include "../Units.h"
#include "../MathTools.h"
#include "../Robot.h"
#include "../RobotConfig.h"
#include "../Nodes/RobotNode.h"
#include "../EndEffector/EndEffectorActor.h"

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <boost/thread.hpp> 

// using forward declarations here, so that the rapidXML header does not have to be parsed when this file is included
namespace rapidxml
{
	template<class Ch>
	class xml_node;
};

namespace VirtualRobot
{

class VIRTUAL_ROBOT_IMPORT_EXPORT BaseIO
{
public:
	static void makeAbsolutePath( const std::string &basePath, std::string &filename );
	static void makeRelativePath( const std::string &basePath, std::string &filename );

protected:

	// instantiation not allowed
	BaseIO();
	virtual ~BaseIO();
	
	static bool isTrue(const char* s);
	static float convertToFloat(const char* s);
 	static void processNodeList(rapidxml::xml_node<char>* parentNode, RobotPtr robot, std::vector<RobotNodePtr>& nodeList, bool clearList = true);
	static void processLimitsNode(rapidxml::xml_node<char> *limitsXMLNode, float &jointLimitLo, float &jointLimitHi);
	static std::string processFileNode(rapidxml::xml_node<char> *fileNode, const std::string &basePath);
	static void processTransformNode(rapidxml::xml_node<char> *transformXMLNode, const std::string &nodeName, Eigen::Matrix4f &transform);
	static Units getUnitsAttribute(rapidxml::xml_node<char> *node, Units::UnitsType u);
	static std::string processNameAttribute(rapidxml::xml_node<char>* node, bool allowOtherAttributes = false);
	static float processFloatAttribute(const std::string &attributeName, rapidxml::xml_node<char>* node, bool allowOtherAttributes = false);
	static float getFloatByAttributeName(rapidxml::xml_node<char>* xmlNode, const std::string& attributeName);
	static float getOptionalFloatByAttributeName(rapidxml::xml_node<char>* xmlNode, const std::string& attributeName, float standardValue);

	static bool processConfigurationNode(rapidxml::xml_node<char>* configXMLNode, std::vector< std::vector< RobotConfig::Configuration > > &configDefinitions, std::vector< std::string > &configNames );

	static std::string getLowerCase(const char* c);
	static void getLowerCase(std::string& aString);
	static std::string processStringAttribute(const std::string &attributeName, rapidxml::xml_node<char>* node, bool allowOtherAttributes = false);

	static VisualizationNodePtr processVisualizationTag(rapidxml::xml_node<char> *visuXMLNode, const std::string &tagName, const std::string &basePath, bool &useAsColModel);
	static CollisionModelPtr processCollisionTag(rapidxml::xml_node<char> *colXMLNode, const std::string &tagName, const std::string &basePath);
	static void processPhysicsTag(rapidxml::xml_node<char> *physicsXMLNode, const std::string &nodeName, SceneObject::Physics &physics);
	static RobotNodeSetPtr processRobotNodeSet(rapidxml::xml_node<char>* setXMLNode, RobotPtr robo, const std::string& robotRootNode, int& robotNodeSetCounter );
	static Eigen::Matrix3f process3x3Matrix(rapidxml::xml_node<char> *matrixXMLNode);
	static bool hasUnitsAttribute(rapidxml::xml_node<char> *node);
	static boost::mutex mutex; 
};

}

#endif // _VirtualRobot_BaseIO_h_
