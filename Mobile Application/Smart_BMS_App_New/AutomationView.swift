import SwiftUI
import Charts
import Firebase
import FirebaseDatabase

struct AutomationView: View {
  
    private var ref = Database.database().reference()
    
    
    @State private var getUpOn = true
    @State private var goodNightOn = false
    @State private var goOutOn = false
    @State private var aRoomOn = false
    @State private var bRoomOn = true
    
    
    @State private var selectedMonth = "Mar"
    @State private var chartData: [ConsumptionData] = []
    
    var body: some View {
        VStack(spacing: 0) {
            headerView
            
            ScrollView {
                VStack(alignment: .leading, spacing: 16) {
                    automationCardsSection
                    
                    Text("Consumption")
                        .font(.headline)
                    
                    HStack {
                        Spacer()
                        menuButton
                    }
                    
                    consumptionChart
                }
                .padding()
            }
        }
        .background(Color(.systemGroupedBackground))
        .ignoresSafeArea(edges: .bottom)
        .onAppear {
            loadChartData()
        }
        
        .onChange(of: getUpOn, sendAutomationData)
        .onChange(of: goodNightOn, sendAutomationData)
        .onChange(of: goOutOn, sendAutomationData)
        .onChange(of: aRoomOn, sendAutomationData)
        .onChange(of: bRoomOn, sendAutomationData)

    }
    
    var headerView: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text("28°C  ☀️   70%  📅  Wed, May 24th")
                    .font(.caption)
                    .foregroundColor(.white)
                Text("Automation")
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
                automationButton(icon: "sun.max.fill", title: "Get up", isOn: $getUpOn)
                automationButton(icon: "moon.fill", title: "Goodnight", isOn: $goodNightOn)
                automationButton(icon: "figure.walk", title: "Go out", isOn: $goOutOn)
            }
            
            HStack(spacing: 16) {
                automationButton(icon: "bed.double.fill", title: "Space A", isOn: $aRoomOn)
                automationButton(icon: "bed.double.fill", title: "Space B", isOn: $bRoomOn)
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
    
    func sendAutomationData() {
        let data: [String: Any] = [
            "getUp": getUpOn,
            "goodNight": goodNightOn,
            "goOut": goOutOn,
            "aRoom": aRoomOn,
            "bRoom": bRoomOn
        ]
        
        ref.child("automationStatus").setValue(data) { error, _ in
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
                let data = value.map { ConsumptionData(month: $0.key, kw: $0.value) }
                    .sorted { $0.month < $1.month }
                chartData = data
            } else {
                print("No chart data found.")
            }
        }
    }
}

struct ConsumptionData: Identifiable {
    let id = UUID()
    let month: String
    let kw: Double
}

struct AutomationView_Previews: PreviewProvider {
    static var previews: some View {
        AutomationView()
    }
}
