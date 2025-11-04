import SwiftUI
import FirebaseDatabase

struct OneFloreView: View {
    let columns = [GridItem(.flexible()), GridItem(.flexible())]
    
    @State private var selectedCabin = "Cabin 1"
    let cabins = ["Cabin 1", "Cabin 2", "Cabin 3", "Cabin 4"]
    
    @State private var cabinDevices: [String: [String: Bool]] = [
        "Cabin 1": ["AirCondition": true, "SmartLight": false, "AirPurifier": true, "LEDLight": false, "HVAC": false, "IAQSensor": true],
        "Cabin 2": ["AirCondition": false, "SmartLight": false, "AirPurifier": true, "LEDLight": false, "HVAC": true, "IAQSensor": false],
        "Cabin 3": ["AirCondition": true, "SmartLight": true, "AirPurifier": false, "LEDLight": true, "HVAC": false, "IAQSensor": false],
        "Cabin 4": ["AirCondition": false, "SmartLight": true, "AirPurifier": true, "LEDLight": false, "HVAC": true, "IAQSensor": true]
    ]
    
    @State private var navigateToDevice: String? = nil
    @State private var lastToggledDevice: String? = nil
    
    let ref = Database.database().reference()
    
    @Environment(\.dismiss) private var dismiss
    
    var body: some View {
        VStack(spacing: 0) {
            headerView
            
            ScrollView {
                VStack(alignment: .leading, spacing: 16) {
                    searchBar
                    welcomeSection
                    roomTabs
                    
                    LazyVGrid(columns: columns, spacing: 20) {
                        deviceCard(imageName: "snow", deviceName: "Air Condition", key: "AirCondition", destination: AnyView(AirConditionView()))
                        deviceCard(imageName: "lightbulb.circle", deviceName: "Smart Light", key: "SmartLight", destination: AnyView(SmartLightView()))
                        deviceCard(imageName: "air.purifier", deviceName: "AirPurifier", destination: nil)
                        deviceCard(imageName: "lightbulb", deviceName: "LED Light", key: "LEDLight", destination: AnyView(DigitalLEDView()))
                        deviceCard(imageName: "wind", deviceName: "HVAC", key: "HVAC", destination: nil)
                        deviceCard(imageName: "thermometer.snowflake", deviceName: "IAQ Sensor", key: "IAQSensor", destination: AnyView(SensorDataView()))
                    }
                    
                    addDeviceButton
                }
                .padding()
            }
            
            NavigationLink(destination: destinationView(for: lastToggledDevice),
                           tag: "navigate",
                           selection: $navigateToDevice) {
                EmptyView()
            }
        }
        .background(Color(.systemGroupedBackground))
        .ignoresSafeArea(edges: .bottom)
        .navigationBarBackButtonHidden(true)
        .toolbar {
            ToolbarItem(placement: .navigationBarLeading) {
                Button(action: {
                    dismiss()
                }) {
                    HStack {
                        Image(systemName: "chevron.left")
                        Text("Back")
                    }
                    .foregroundColor(.black)
                }
            }
        }
        .onChange(of: cabinDevices) { _ in sendDeviceStates() }
    }
    
    func sendDeviceStates() {
        for (cabin, devices) in cabinDevices {
            ref.child("First Floor").child(cabin).setValue(devices)
        }
    }
    
    var headerView: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text("First Floor")
                    .font(.largeTitle.bold())
                    .foregroundColor(.white)
            }
            Spacer()
            Image(systemName: "person.crop.circle")
                .resizable()
                .frame(width: 40, height: 40)
                .clipShape(Circle())
        }
        .padding()
        .background(Color.blue)
    }
    
    var searchBar: some View {
        HStack {
            Image(systemName: "magnifyingglass")
                .foregroundColor(.gray)
            TextField("Search...", text: .constant(""))
        }
        .padding()
        .background(Color.white)
        .cornerRadius(10)
    }
    
    var welcomeSection: some View {
        HStack {
            VStack(alignment: .leading) {
                Text("Take control as you begin your seamless journey of home automation")
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            Spacer()
        }
    }
    
    var roomTabs: some View {
        HStack {
            ForEach(cabins, id: \.self) { cabin in
                Button(action: { selectedCabin = cabin }) {
                    Text(cabin)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 10)
                        .background(selectedCabin == cabin ? Color.blue : Color.blue.opacity(0.1))
                        .foregroundColor(selectedCabin == cabin ? .white : .blue)
                        .cornerRadius(10)
                }
            }
        }
    }
    
    var addDeviceButton: some View {
        Button(action: {
            print("Add device tapped")
        }) {
            HStack {
                Image(systemName: "plus.circle.fill")
                Text("Add Device")
            }
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.green)
            .foregroundColor(.white)
            .cornerRadius(12)
        }
        .padding(.top, 20)
    }
    
    func deviceCard(imageName: String, deviceName: String, key: String? = nil, destination: AnyView?) -> some View {
        VStack(spacing: 12) {
            Image(systemName: imageName)
                .resizable()
                .aspectRatio(contentMode: .fit)
                .frame(height: 50)
                .foregroundColor(.blue)
            
            Text(deviceName)
                .font(.headline)
                .foregroundColor(.primary)
            
            if let key = key {
                Button(action: {
                    lastToggledDevice = key
                    navigateToDevice = "navigate"
                }) {
                    Image(systemName: "chevron.right.circle.fill")
                        .resizable()
                        .frame(width: 30, height: 30)
                        .foregroundColor(.blue)
                }
            } else {
                Image(systemName: "chevron.right.circle")
                    .resizable()
                    .frame(width: 30, height: 30)
                    .foregroundColor(.gray)
                    .opacity(0.5)
            }
        }
        .padding()
        .frame(maxWidth: .infinity)
        .background(Color.white)
        .cornerRadius(20)
        .shadow(radius: 3)
    }

    
    @ViewBuilder
    func destinationView(for deviceKey: String?) -> some View {
        switch deviceKey {
        case "AirCondition":
            AirConditionView()
        case "SmartLight":
            SmartLightView()
        case "LEDLight":
            DigitalLEDView()
        case "IAQSensor":
            SensorDataView()
        default:
            EmptyView()
        }
    }
}

struct OneFloreView_Previews: PreviewProvider {
    static var previews: some View {
        NavigationStack {
            OneFloreView()
        }
    }
}
