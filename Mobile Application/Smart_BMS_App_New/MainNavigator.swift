import SwiftUI

struct MainAppView: View {
    @State private var path = NavigationPath()

    var body: some View {
        NavigationStack(path: $path) {
            VStack(spacing: 0) {
            }
            .ignoresSafeArea(edges: .bottom)
            .navigationDestination(for: String.self) { page in
                switch page {
                case "automation":
                    BuildingAutomationView()
                case "floor1":
                    OneFloreView()
                case "floor2":
                    OneFloreView()
                case "floor3":
                    OneFloreView()
                case "floor4":
                    OneFloreView()
                default:
                    EmptyView()
                }
            }
        }
    }
}


