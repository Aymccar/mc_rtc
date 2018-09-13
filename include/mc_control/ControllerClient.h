#pragma once

#include <mc_control/client_api.h>

#include <mc_rtc/Configuration.h>
#include <mc_rtc/GUITypes.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/reqrep.h>

#include <string>
#include <thread>
#include <vector>

namespace mc_control
{

  /** Used to uniquely identify an element */
  struct MC_CONTROL_CLIENT_DLLAPI ElementId
  {
    /** Category the element belongs to */
    std::vector<std::string> category;
    /** Name of the element */
    std::string name;
  };

  /** Receives data and interact with a ControllerServer
   *
   * - Uses a SUB socket to receive the data stream
   *
   * - Uses a REQ socket to send requests
   */
  struct MC_CONTROL_CLIENT_DLLAPI ControllerClient
  {

    /** Constructor
     *
     * \param sub_conn_uri URI the SUB socket should connect to
     *
     * \param push_conn_uri URI the PUSH socket should connect to
     *
     * \param timeout After timeout has elapsed without receiving messages from
     * the SUB socket, pass an empty message to handle_gui_state. It should be
     * expressed in secondd. If timeout <= 0, this is ignored.
     *
     * Check nanomsg documentation for supported protocols
     */
    ControllerClient(const std::string & sub_conn_uri,
                     const std::string & push_conn_uri,
                     double timeout = 0);

    ControllerClient(const ControllerClient &) = delete;
    ControllerClient & operator=(const ControllerClient &) = delete;

    ~ControllerClient();

    /** Send a request to the given element in the given category using data */
    void send_request(const ElementId & id, const mc_rtc::Configuration & data);

    /** Helper for send_request in simple cases */
    template<typename T>
    void send_request(const ElementId & id, const T & data)
    {
      mc_rtc::Configuration c;
      c.add("data", data);
      send_request(id, c("data"));
    }

    /** Helper for the void case */
    void send_request(const ElementId & id);

    /** Set the timeout of the SUB socket */
    double timeout(double t);
  protected:
    /** Should be called when the client is ready to receive data */
    void start();

    void handle_gui_state(const char * data);

    void handle_category(const std::vector<std::string> & parent, const std::string & category, const mc_rtc::Configuration & data);

    void handle_widget(const ElementId & id, const mc_rtc::Configuration & data);

    /** Called when a message starts being processed, can be used to lock the GUI */
    virtual void started() {}

    /** Called when a message has been processed */
    virtual void stopped() {}

    /** Should be implemented to create a new category container */
    virtual void category(const std::vector<std::string> & parent, const std::string & category) = 0;

    /** Should be implemented to create a label for data that can be displayed as string */
    inline virtual void label(const ElementId & id, const std::string &)
    {
      default_impl("Label", id);
    }

    /** Should be implemented to create a label for a numeric array
     *
     * \p category Category under which the label appears
     * \p label Name of the data
     * \p labels Per-dimension label (can be empty)
     * \p data Data to display
     */
    inline virtual void array_label(const ElementId & id,
                             const std::vector<std::string> &,
                             const Eigen::VectorXd &)
    {
      default_impl("ArrayLabel", id);
    }

    /** Should be implemented to create a button */
    virtual void button(const ElementId & id)
    {
      default_impl("Button", id);
    }

    /** Should be implemented to create a checkbox */
    virtual void checkbox(const ElementId & id,
                          bool /*state */)
    {
      default_impl("Checkbox", id);
    }

    /** Should be implemented to create a widget able to input strings */
    virtual void string_input(const ElementId & id,
                              const std::string & /*data*/)
    {
      default_impl("StringInput", id);
    }

    /** Should be implemented to create a widget able to input integers */
    virtual void integer_input(const ElementId & id,
                               int /*data*/)
    {
      default_impl("IntegerInput", id);
    }

    /** Should be implemented to create a widget able to input numbers */
    virtual void number_input(const ElementId & id,
                              double /*data*/)
    {
      default_impl("NumberInput", id);
    }

    virtual void number_slider(const ElementId & id,
                               double /*data*/, double /*min*/, double /*max*/)
    {
      default_impl("NumberSlider", id);
    }

    /** Should be implemented to create a widget able to input array of numbers */
    virtual void array_input(const ElementId & id,
                             const std::vector<std::string> & /*labels*/,
                             const Eigen::VectorXd & /*data*/)
    {
      default_impl("ArrayInput", id);
    }

    /** Should be implemented to create a widget able to select one string among many */
    virtual void combo_input(const ElementId & id,
                             const std::vector<std::string> & /*values*/,
                             const std::string & /*data*/)
    {
      default_impl("ComboInput", id);
    }

    /** Should be implemented to create a widget able to select one string
     * among entries available in the data part of the GUI message */
    virtual void data_combo_input(const ElementId & id,
                                  const std::vector<std::string> & /*data_ref*/,
                                  const std::string & /*data*/)
    {
      default_impl("DataComboInput", id);
    }

    /** Should display a point in 3D environment
     *
     * \p requestId should be in requests instead of \p id
     *
     * bool \p ro indicates whether this point is interactive or not
     */
    virtual void point3d(const ElementId & id,
                         const ElementId & /*requestId*/,
                         bool /*ro */,
                         const Eigen::Vector3d & /*pos*/)
    {
      default_impl("Point3D", id);
    }

    /** Should display a trajectory of 3d points in 3D environment
     *
     * \p Vector of 3D points
     */
    virtual void trajectory(const ElementId & id,
                            const std::vector<Eigen::Vector3d> & /* points */)
    {
      default_impl("Point3DTrajectory", id);
    }

    /** Should display a trajectory of transforms in 3D environment
     *
     * \p Vector of 3D points
     */
    virtual void trajectory(const ElementId & id,
                            const std::vector<sva::PTransformd> & /* points */)
    {
      default_impl("PoseTrajectory", id);
    }

    /** Should display a polygon of 3d points in 3D environment
     *
     * \p Vector of 3D points
     */
    virtual void polygon(const ElementId & id,
                         const std::vector<Eigen::Vector3d> & /* points */,
                         const mc_rtc::gui::Color& /* color */)
    {
      default_impl("Polygon", id);
    }

    /** Should display a force in 3D environment
     */
    virtual void force(const ElementId & id,
                       const ElementId & /*requestId*/,
                       const sva::ForceVecd & /* force */,
                       const sva::PTransformd & /* surface */,
                       const mc_rtc::gui::ForceConfig & /* forceConfig */)
    {
      default_impl("Force", id);
    }

    /** Should display an arrow in 3D environment
     */
    virtual void arrow(const ElementId & id,
                       const Eigen::Vector3d & /* start */,
                       const Eigen::Vector3d & /* end */,
                       const mc_rtc::gui::ArrowConfig & /* config */)
    {
      default_impl("Arrow", id);
    }

    /** Should display a rotation in 3D environment
     *
     * \p requestId should be in requests instead of \p id
     *
     * bool \p ro indicates whether this point is interactive or not
     */
    virtual void rotation(const ElementId & id,
                          const ElementId & /*requestId*/,
                          bool /*ro */,
                          const sva::PTransformd & /*pos*/)
    {
      default_impl("Rotation", id);
    }

    /** Should display a PTransform in 3D environment
     *
     * \p requestId should be in requests instead of \p id
     *
     * bool \p ro indicates whether this point is interactive or not
     */
    virtual void transform(const ElementId & id,
                           const ElementId & /*requestId*/,
                           bool /*ro */,
                           const sva::PTransformd & /*pos*/)
    {
      default_impl("Transform", id);
    }

    /** Should display a form to send schema-based request to the server
     *
     * \p schema is the schema directory relative to mc_rtc JSON schema installation
     */
    virtual void schema(const ElementId & id,
                        const std::string & /*schema*/)
    {
      default_impl("Schema", id);
    }

    /** Create a form */
    virtual void form(const ElementId & id)
    {
      default_impl("Form", id);
    }

    /** A checkbox within a form */
    virtual void form_checkbox(const ElementId & /*formId*/,
                               const std::string & /*name*/,
                               bool /*required*/,
                               bool /*default*/) {}

    /** An integer input within a form */
    virtual void form_integer_input(const ElementId & /*formId*/,
                                    const std::string & /*name*/,
                                    bool /*required*/,
                                    int /*default*/) {}

    /** A number input within a form */
    virtual void form_number_input(const ElementId & /*formId*/,
                                   const std::string & /*name*/,
                                   bool /*required*/,
                                   double /*default*/) {}

    /** A string input within a form */
    virtual void form_string_input(const ElementId & /*formId*/,
                                   const std::string & /*name*/,
                                   bool /*required*/,
                                   const std::string & /*default*/) {}

    /** An array input within a form */
    virtual void form_array_input(const ElementId & /*formId*/,
                                  const std::string & /*name*/,
                                  bool /*required*/,
                                  const Eigen::VectorXd & /*default*/,
                                  bool /*fixed_size*/) {}

    /** A combo input within a form
     *
     * \p formId Identifier of the form
     *
     * \p name Name of the entry
     *
     * \p required If true, it must hold a value when the form is sent
     *
     * \p values Possible values
     *
     * \p send_index If true, the implementation should send back the index
     * rather than the value
     */
    virtual void form_combo_input(const ElementId & /*formId*/,
                                  const std::string & /*name*/,
                                  bool /*required*/,
                                  const std::vector<std::string> & /*values*/,
                                  bool /*send_index*/) {}

    /** A data combo input within a form
     *
     * \p formId Identifier of the form
     *
     * \p name Name of the entry
     *
     * \p required If true, it must hold a value when the form is sent
     *
     * \p ref Reference to GUI data store, can reference other fields of the
     * form, e.g. {"$R0", "surfaces"} depends on the value of the R0 entry in
     * the form
     *
     * \p send_index If true, the implementation should send back the index in
     * the list rather than the value
     */
    virtual void form_data_combo_input(const ElementId & /*formId*/,
                                       const std::string & /*name*/,
                                       bool /*required*/,
                                       const std::vector<std::string> & /*ref*/,
                                       bool /*send_index*/) {}

    /* Network elements */
    bool run_ = true;
    int sub_socket_;
    std::thread sub_th_;
    int push_socket_;
    double timeout_;

    /* Hold data from the server */
    mc_rtc::Configuration data_;
  private:
    /** Default implementations for widgets' creations display a warning message to the user */
    void default_impl(const std::string & type, const ElementId & id);

    /** Handle details of Point3D elements */
    void handle_point3d(const ElementId & id,
                        const mc_rtc::Configuration & gui,
                        const mc_rtc::Configuration & data);

    /** Handle details of DisplayPoint3DTrajectory elements */
    void handle_point3DTrajectory(const ElementId & id,
                        const mc_rtc::Configuration & gui,
                        const mc_rtc::Configuration & data);

    /** Handle details of DisplayPoseTrajectory elements */
    void handle_poseTrajectory(const ElementId & id,
                        const mc_rtc::Configuration & gui,
                        const mc_rtc::Configuration & data);

    /** Hand details of DisplayPolygon elemets */
    void handle_polygon(const ElementId & id,
                               const mc_rtc::Configuration & gui,
                               const mc_rtc::Configuration & data);

    /** Hand details of DisplayForce elemets */
    void handle_force(const ElementId & id,
                             const mc_rtc::Configuration & gui,
                             const mc_rtc::Configuration & data);

    /** Hand details of DisplayArrow elemets */
    void handle_arrow(const ElementId & id,
                             const mc_rtc::Configuration & gui,
                             const mc_rtc::Configuration & data);

    /** Handle details of Rotation elements */
    void handle_rotation(const ElementId & id,
                         const mc_rtc::Configuration & gui,
                         const mc_rtc::Configuration & data);

    /** Handle details of Transform elements */
    void handle_transform(const ElementId & id,
                          const mc_rtc::Configuration & gui,
                          const mc_rtc::Configuration & data);

    /** Handle details of Form elements */
    void handle_form(const ElementId & id,
                     const mc_rtc::Configuration & gui);
  };


}
