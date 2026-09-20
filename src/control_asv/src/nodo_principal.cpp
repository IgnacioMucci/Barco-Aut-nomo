
// funciones de tiempo, memoria y listas de texto.
#include <chrono>
#include <memory>
#include <vector>

// herramientas principales de ROS 2 para C++, nos permite crear "Nodos".
#include "rclcpp/rclcpp.hpp"
// Esta herramienta nos permite crear un mensaje que es una lista de números enteros (Int32MultiArray).
// Usamos esta lista para enviar los valores de los canales (1500, 1600, etc.).
#include "std_msgs/msg/int32_multi_array.hpp"

// Esto nos permite usar ms para escribir "100ms".
using namespace std::chrono_literals;

// creamos nuestro "operario"
// "public rclcpp::Node" significa que hereda todas las habilidades de un Nodo de ROS 2.
class NodoPrincipal : public rclcpp::Node
{
// "public:" significa que las funciones pueden ser usadas por cualquiera.
public:
  // Este es el "constructor". Es lo primero que hace.
  NodoPrincipal() : Node("nodo_principal_asv") // nombre "nodo_principal_asv"
  {
    // mensajes del tipo lista de números (Int32MultiArray).
    // El tópic se llama "/ap/rc/override".
    // El "10" es el tamaño de la fila de espera por si los mensajes se acumulan.
    pwm_publisher_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/ap/rc/override", 10);
    
    // Le damos al operario un  timer.
    // Cada 100 ms ejecuta la función llamada 'enviar_pwm'".
    timer_ = this->create_wall_timer(
      100ms, std::bind(&NodoPrincipal::enviar_pwm, this));
      
    // Imprime un mensaje en la terminal.
    RCLCPP_INFO(this->get_logger(), "Enviando señales PWM provisionales a los T200...");
  }

// "private:" significa que lo de abajo son de uso interno exclusivo del operario.
private:
  // Esta es la tarea repetitiva que el timer activa 10 veces por segundo.
  void enviar_pwm()
  {
    // mensaje vacío
    auto mensaje = std_msgs::msg::Int32MultiArray();
    
    // Creamos la lista llamada canales que tiene 8 espacios (simulando los 8 canales de un control remoto).
    // El ", 0" hace que los 8 espacios empiecen rellenados con el número 0.
    std::vector<int32_t> canales(8, 0);
    
    // Cambiamos el valor del primer canal a 1600.
    // Esto le dice al propulsor izquierdo que avance suavemente.
    canales[0] = 1600; 
    
    // Cambiamos el valor del tercer canal a 1600.
    canales[2] = 1600; 
    
    // Metemos nuestra lista de 8 numeros dentro del mensaje que preparamos antes.
    mensaje.data = canales;
    
    // Usamos nuestro publisher para gritar publicar el mensaje al sistema de ROS 2.
    pwm_publisher_->publish(mensaje);
  }
  
  // variables internas de el cronómetro y el publisher para que no se borren de la memoria.
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr pwm_publisher_;
};

// Es lo primero que ejecuta la computadora.
int main(int argc, char * argv[])
{
  // 1. Inicializa el sistema de comunicaciones de ROS 2.
  rclcpp::init(argc, argv);
  
  // 2. Crea nuestro NodoPrincipa y lo pone a trabajar en un bucle infinito (spin).
  // Se quedará ahí publicando mensajes 10 veces por segundo hasta que apretemos Ctrl+C.
  rclcpp::spin(std::make_shared<NodoPrincipal>());
  
  // 3. Si apretamos Ctrl+C, el código sale del bucle, limpia la memoria, apaga ROS 2 y cierra el programa.
  rclcpp::shutdown();
  return 0;
}