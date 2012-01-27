//----------------------------------------------------------------------
/*!
 * \ingroup Saba
 *
 *
 * \author  Nikolaus Vahrenkamp
 * \date    2011-04-04
 *
 * \class CoinRrtWorkspaceVisualization
 *
 * A Coin3D related visualization of an RRT search tree.
 *
 * Copyright Nikolaus Vahrenkamp
 * Karlsruhe Institute of Technology (KIT) - HIS

 */
//----------------------------------------------------------------------


#ifndef _Saba_CoinRrtWorkspaceVisualization_h_
#define _Saba_CoinRrtWorkspaceVisualization_h_


#include "../../Saba.h"
#include "../RrtWorkspaceVisualization.h"

#include <boost/shared_ptr.hpp>

class SoNode;
class SoSeparator;
class SoCallbackAction;
class SoPrimitiveVertex;

namespace Saba
{

class SABA_IMPORT_EXPORT CoinRrtWorkspaceVisualization : virtual public RrtWorkspaceVisualization
{
public:
	/*!
		Constructor
		Robot must have a node with name TCPName. 
		The visualizations are build by determining the TCP's position in workspace according to the configurations of a path or tree .
	*/
	CoinRrtWorkspaceVisualization(VirtualRobot::RobotPtr robot, CSpacePtr cspace, const std::string &TCPName);
	CoinRrtWorkspaceVisualization(VirtualRobot::RobotPtr robot, VirtualRobot::RobotNodeSetPtr robotNodeSet, const std::string &TCPName);

	~CoinRrtWorkspaceVisualization();

	enum ColorSet
	{
		eRed,
		eGreen,
		eBlue,
		eCustom
	};
	
	/*!
		Add visualization of a path in cspace.
	*/
	virtual bool addCSpacePath(CSpacePathPtr path, CoinRrtWorkspaceVisualization::ColorSet colorSet = eBlue);
	void setPathStyle(float lineSize = 4.0f, float nodeSize= 15.0f, float renderComplexity = 1.0f);

	/*!
		Add visualization of a path in cspace.
	*/
	virtual bool addTree(CSpaceTreePtr tree, CoinRrtWorkspaceVisualization::ColorSet colorSet = eRed);
	void setTreeStyle(float lineSize = 1.0f, float nodeSize= 15.0f, float renderComplexity = 0.1f);
	
	/*!
		Add visualization of a configuration in cspace.
	*/
	virtual bool addConfiguration(const Eigen::VectorXf &c, CoinRrtWorkspaceVisualization::ColorSet colorSet = eGreen, float nodeSizeFactor = 1.0f);

	/*!
		Set the custom line and node color. Does not affect already added trees or paths.
	*/
	void setCustomColor(float nodeR, float nodeG, float nodeB, float lineR = 0.5f, float lineG = 0.5f, float lineB = 0.5f);

	/*!
		Clears all visualizations.
	*/
	virtual void reset();


	SoSeparator* getCoinVisualization();

protected:

	void init();
	
	SoSeparator* visualization;

	float pathLineSize, pathNodeSize, pathRenderComplexity;
	float treeLineSize, treeNodeSize, treeRenderComplexity;

	struct RenderColors
	{
		float nodeR, nodeG, nodeB, lineR, lineG, lineB;
	};

	std::map<ColorSet, RenderColors> colors;
};

} // namespace Saba

#endif // _Saba_CoinRrtWorkspaceVisualization_h_