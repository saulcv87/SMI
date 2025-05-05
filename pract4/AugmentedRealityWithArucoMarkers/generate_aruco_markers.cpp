#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Usamos el diccionario que espera el código principal
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_50);

    // Lista de IDs de los marcadores requeridos por el ejemplo
    vector<int> markerIds = {25, 33, 30, 23};

    for (int id : markerIds) {
        Mat markerImage;
        int markerSize = 200;  // Tamaño en píxeles de la imagen generada

        // Generar el marcador
        aruco::drawMarker(dictionary, id, markerSize, markerImage, 1);

        // Crear nombre de archivo según el esquema marker<ID>.png
        string filename = "marker" + to_string(id) + ".png";
        imwrite(filename, markerImage);

        cout << "Generado: " << filename << endl;
    }

    return 0;
}

