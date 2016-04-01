#pragma once

#include <map>
#include <string>
#include <vector>

#include <mc_rbdyn/robot.h>

#include <mc_control/api.h>

namespace mc_control
{

/*! \brief Stores mimic joint information */
struct MC_CONTROL_DLLAPI Mimic
{
public:
  /*! Which joint this joint mimics */
  std::string joint;
  /*! Mimic multiplier (usually -1/+1) */
  double multiplier;
  /*! Mimic offset */
  double offset;
};

/*! \brief A collection of mimic joints
 *
 * Keys are mimic joints' names
 *
 * Values are mimic joint information
 */
typedef std::map<std::string, Mimic> mimic_d_t;

/*! \brief A robot's gripper reprensentation
 *
 * A gripper is composed of a set of joints that we want to
 * control only through "manual" operations. It may include
 * passive joints.
 *
 * In real operations the actuated joints are also
 * monitored to avoid potential servo errors.
 */
struct MC_CONTROL_DLLAPI Gripper
{
public:
  /*! \brief Constructor
   *
   * \param robot The full robot including uncontrolled joints
   * \param jointNames Name of the active joints involved in the gripper
   * \param robot_urdf URDF of the robot
   * \param currentQ Current values of the active joints involved in the gripper
   * \param timeStep Controller timestep
   */
  Gripper(const mc_rbdyn::Robot & robot, const std::vector<std::string> & jointNames,
          const std::string & robot_urdf,
          const std::vector<double> & currentQ, double timeStep);

  /*! \brief Set the current configuration of the active joints involved in the gripper
   * \param curentQ Current values of the active joints involved in the gripper
   */
  void setCurrentQ(const std::vector<double> & currentQ);

  /*! \brief Set the target configuration of the active joints involved in the gripper
   * \param targetQ Desired values of the active joints involved in the gripper
   */
  void setTargetQ(const std::vector<double> & targetQ);

  /*! \brief Set the target opening of the gripper
   * \param targetOpening Opening value ranging from 0 (closed) to 1 (open)
   */
  void setTargetOpening(double targetOpening);

  /*! \brief Get current configuration
   * \return Current values of the active joints involved in the gripper
   */
  std::vector<double> curPosition() const;

  /*! \brief Return all gripper joints configuration
   * \return Current values of all the gripper's joints, including passive joints
   */
  const std::vector<double> & q();

  /*! \brief Set the encoder-based values of the gripper's active joints
   * \param q Encoder value of the gripper's active joints
   */
  void setActualQ(const std::vector<double> & q);
public:
  /*! Gripper name */
  std::string name;
  /*! Name of joints involved in the gripper */
  std::vector<std::string> names;
  /*! Name of active joints involved in the gripper */
  std::vector<std::string> active_joints;

  /*! Lower limits of active joints in the gripper (closed-gripper values) */
  std::vector<double> closeP;
  /*! Upper limits of active joints in the gripper (open-gripper values) */
  std::vector<double> openP;
  /*! Maximum velocity of active joints in the gripper */
  std::vector<double> vmax;
  /*! Controller timestep */
  double timeStep;
  /*! Current opening percentage */
  std::vector<double> percentOpen;

  /*! Active joints */
  std::vector<size_t> active_idx;
  /*! Mimic multiplier, first element is the joint to mimic, second is the multiplier */
  std::vector< std::pair<size_t, double> > mult;
  /*! Full joints' values */
  std::vector<double> _q;

  /*! Current gripper target */
  std::vector<double> targetQIn;
  /*! Current gripper target: NULL if target has been reached or safety was triggered */
  std::vector<double> * targetQ;

  /*! True if the gripper has been too far from the command for over overCommandLimitIterN iterations */
  std::vector<bool> overCommandLimit;
  /*! Store the number of iterations where the gripper command was over the limit */
  std::vector<unsigned int> overCommandLimitIter;
  /*! Number of iterations before the security is triggered */
  unsigned int overCommandLimitIterN;
  /*! Joints' values from the encoders */
  std::vector<double> actualQ;
  /*! Difference between the command and the reality that triggers the safety */
  double actualCommandDiffTrigger;
};

} // namespace mc_control
