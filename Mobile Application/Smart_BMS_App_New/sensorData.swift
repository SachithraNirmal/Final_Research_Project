import SwiftUI
import FirebaseDatabase

struct SensorDataView: View {
    @State private var temperature: Double = 25.0
    @State private var humidity: Int = 60
    
    @Environment(\.dismiss) private var dismiss
    
    private let ref = Database.database().reference()
    
    var body: some View {
        VStack(spacing: 0) {
            
            headerView
            
            VStack(spacing: 40) {
                sensorCircle(
                    value: temperature,
                    maxValue: 50,
                    gradientColors: [.blue, .cyan],
                    systemImage: "thermometer.sun.fill",
                    label: "Temperature",
                    unit: "°C",
                    color: .blue
                )
                
                sensorCircle(
                    value: Double(humidity),
                    maxValue: 100,
                    gradientColors: [.green, .mint],
                    systemImage: "humidity.fill",
                    label: "Humidity",
                    unit: "%",
                    color: .green
                )
            }
            .padding(.vertical, 40)
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .background(Color(.systemGray6))
            .onAppear {
                loadFromFirebase()
            }
        }
        .ignoresSafeArea(edges: .top)
        .navigationBarBackButtonHidden(true)
    }
    
    var headerView: some View {
        ZStack(alignment: .topLeading) {
            LinearGradient(
                gradient: Gradient(colors: [Color.blue, Color.cyan]),
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            )
            .frame(height: 180)
            .shadow(color: .black.opacity(0.2), radius: 8, x: 0, y: 4)
            
            VStack(alignment: .leading, spacing: 10) {
                Button(action: { dismiss() }) {
                    HStack(spacing: 6) {
                        Image(systemName: "chevron.left")
                        Text("Back")
                            .fontWeight(.medium)
                    }
                    .foregroundColor(.white)
                }
                .padding(.top, 50)
                .padding(.horizontal)
                
                VStack(alignment: .leading, spacing: 4) {
                    Text("IAQ Sensor")
                        .font(.largeTitle.bold())
                        .foregroundColor(.white)
                    
                    Text("(Indoor Air Quality Sensor)")
                        .font(.subheadline)
                        .foregroundColor(.white.opacity(0.9))
                }
                .padding(.horizontal)
            }
        }
    }
    
    func sensorCircle(
        value: Double,
        maxValue: Double,
        gradientColors: [Color],
        systemImage: String,
        label: String,
        unit: String,
        color: Color
    ) -> some View {
        ZStack {
            Circle()
                .stroke(Color.gray.opacity(0.2), lineWidth: 12)
            
            Circle()
                .trim(from: 0, to: CGFloat(min(value / maxValue, 1.0)))
                .stroke(
                    AngularGradient(
                        gradient: Gradient(colors: gradientColors),
                        center: .center
                    ),
                    style: StrokeStyle(lineWidth: 12, lineCap: .round)
                )
                .rotationEffect(.degrees(-90))
                .shadow(radius: 3)
            
            VStack(spacing: 8) {
                Image(systemName: systemImage)
                    .font(.largeTitle)
                    .foregroundColor(color)
                Text(String(format: "%.1f%@", value, unit))
                    .font(.system(size: 32, weight: .bold))
                Text(label)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
        }
        .frame(width: UIScreen.main.bounds.width * 0.45,
               height: UIScreen.main.bounds.width * 0.45)
    }
    
    func loadFromFirebase() {
        ref.child("sensorData").observe(.value) { snapshot in
            if let value = snapshot.value as? [String: Any] {
                self.temperature = value["temperature"] as? Double ?? 25.0
                self.humidity = value["humidity"] as? Int ?? 60
            }
        }
    }
}

#Preview {
    NavigationStack {
        SensorDataView()
    }
}
