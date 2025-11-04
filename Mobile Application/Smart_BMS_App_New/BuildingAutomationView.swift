import SwiftUI
import Charts
import Firebase
import FirebaseDatabase

struct BuildingAutomationView: View {
    
    private var ref = Database.database().reference()
    
    @State private var getUpOn = true
    @State private var goodNightOn = false
    @State private var goOutOn = false
    @State private var lobbyOn = false
    @State private var conferenceRoomOn = true
    
    @State private var selectedMonth = "Mar"
    @State private var chartData: [BuildingConsumptionData] = []
    
    @State private var showHighConsumptionAlert = false
    @State private var highConsumptionValue: Double = 0
    
    var body: some View {
        NavigationStack {
            VStack(spacing: 0) {
                
                headerView
                
                ScrollView {
                    VStack(alignment: .leading, spacing: 16) {
                        automationCardsSection
                        
                        Text("Energy Consumption")
                            .font(.headline)
                        
                        HStack {
                            Spacer()
                            menuButton
                        }
                        
                        consumptionChart
                    }
                    .padding()
                }
                
                Divider()
                
                bottomNavigationBar
            }
            .background(Color(.systemGroupedBackground))
            .ignoresSafeArea(edges: .bottom)
            .onAppear {
                loadChartData()
            }
            .onChange(of: getUpOn, sendAutomationData)
            .onChange(of: goodNightOn, sendAutomationData)
            .onChange(of: goOutOn, sendAutomationData)
            .onChange(of: lobbyOn, sendAutomationData)
            .onChange(of: conferenceRoomOn, sendAutomationData)
            
            .alert("High Energy Consumption", isPresented: $showHighConsumptionAlert, actions: {
                Button("OK", role: .cancel) { }
            }, message: {
                Text("Consumption has exceeded 80 kW! Current: \(highConsumptionValue, specifier: "%.2f") kW")
            })
        }
    }
    
    var headerView: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text("28°C  ☀️   70%  📅  Wed, May 24th")
                    .font(.caption)
                    .foregroundColor(.white)
                Text("Building Automation")
                    .font(.largeTitle.bold())
                    .foregroundColor(.white)
            }
            Spacer()
            Image(systemName: "plus")
                .foregroundColor(.white)
                .font(.title2)
            Image(systemName: "person.crop.circle")
                .resizable()
                .frame(width: 40, height: 40)
                .clipShape(Circle())
        }
        .padding()
        .background(Color.blue)
    }
    
    var automationCardsSection: some View {
        VStack(spacing: 16) {
            HStack(spacing: 16) {
                automationButton(icon: "sunrise.fill", title: "Floors", isOn: $getUpOn)
                automationButton(icon: "moon.stars.fill", title: "Cabins", isOn: $goodNightOn)
                automationButton(icon: "figure.walk.circle.fill", title: "Away Mode", isOn: $goOutOn)
            }
            
            HStack(spacing: 16) {
                automationButton(icon: "building.2.fill", title: "Lobby", isOn: $lobbyOn)
                automationButton(icon: "person.3.fill", title: "Conference Room", isOn: $conferenceRoomOn)
            }
        }
    }
    
    func automationButton(icon: String, title: String, isOn: Binding<Bool>) -> some View {
        VStack(spacing: 8) {
            Image(systemName: icon)
                .font(.largeTitle)
                .foregroundColor(.blue)
            Text(title)
                .font(.headline)
            Toggle("", isOn: isOn)
                .labelsHidden()
        }
        .padding()
        .frame(maxWidth: .infinity)
        .background(Color.white)
        .cornerRadius(20)
        .shadow(radius: 3)
    }
    
    var menuButton: some View {
        Menu {
            Button("Months", action: {})
            Button("Weeks", action: {})
            Button("Days", action: {})
        } label: {
            Label("Months", systemImage: "chevron.down")
                .padding(8)
                .background(Color.blue.opacity(0.1))
                .cornerRadius(8)
        }
    }
    
    var consumptionChart: some View {
        Chart {
            ForEach(chartData) { item in
                LineMark(
                    x: .value("Month", item.month),
                    y: .value("kW", item.kw)
                )
                .symbol(by: .value("Selected", item.month == selectedMonth ? "Selected" : ""))
            }
        }
        .frame(height: 200)
        .padding(.top, 8)
    }
    
    var bottomNavigationBar: some View {
        HStack {
            Spacer()
            navButton(icon: "bolt.circle", label: "Automation", destination: BuildingAutomationView())
            Spacer()
            navButton(icon: "building.2.fill", label: "Floor 1", destination: OneFloreView())
            Spacer()
            navButton(icon: "building.2.fill", label: "Floor 2", destination: Floor2View())
            Spacer()
            navButton(icon: "building.2.fill", label: "Floor 3", destination: Floor3View())
            Spacer()
            navButton(icon: "server.rack", label: "Floor 4", destination: Floor4View())
            Spacer()
        }
        .padding(.vertical, 10)
        .background(Color(.systemGray6))
        .shadow(radius: 2)
    }
    
    func navButton<Destination: View>(icon: String, label: String, destination: Destination) -> some View {
        NavigationLink(destination: destination) {
            VStack(spacing: 4) {
                Image(systemName: icon)
                    .font(.title2)
                    .foregroundColor(.blue)
                Text(label)
                    .font(.caption)
                    .foregroundColor(.blue)
            }
        }
    }
    
    func sendAutomationData() {
        let data: [String: Any] = [
            "getUp": getUpOn,
            "goodNight": goodNightOn,
            "goOut": goOutOn,
            "lobby": lobbyOn,
            "conferenceRoom": conferenceRoomOn
        ]
        
        ref.child("buildingAutomationStatus").setValue(data) { error, _ in
            if let error = error {
                print("Failed to send: \(error.localizedDescription)")
            } else {
                print("Automation data sent to Firebase.")
            }
        }
    }
    
    func loadChartData() {
        ref.child("consumption").observeSingleEvent(of: .value) { snapshot in
            if let value = snapshot.value as? [String: Double] {
                let data = value.map { BuildingConsumptionData(month: $0.key, kw: $0.value) }
                    .sorted { $0.month < $1.month }
                chartData = data
                
                if let high = data.first(where: { $0.kw > 80 }) {
                    highConsumptionValue = high.kw
                    showHighConsumptionAlert = true
                }
            } else {
                print("No chart data found.")
            }
        }
    }
}

struct BuildingConsumptionData: Identifiable {
    let id = UUID()
    let month: String
    let kw: Double
}

struct Floor2View: View { var body: some View { SecondFloorView() } }
struct Floor3View: View { var body: some View { ThirdFloorView() } }
struct Floor4View: View { var body: some View { FourthFloorView() } }

struct BuildingAutomationView_Previews: PreviewProvider {
    static var previews: some View {
        BuildingAutomationView()
    }
}
