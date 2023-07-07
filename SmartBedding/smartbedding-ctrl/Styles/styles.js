import { StyleSheet } from "react-native";

// styles object
const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "flex-start",
  },
  header: {
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "flex-start",
  },
  title: {
    fontSize: 20,
    paddingTop: 25,
  },

  textBox: {
    alignItems: "center",
    marginVertical: 5,
  },
  textBoxText: {
    fontStyle: "italic",
    fontSize: 12,
  },
  listBox: {
    flex: 3,
    backgroundColor: "#f2f2f2",
    alignItems: "flex-start",
    marginVertical: 10,
    width: 300,
  },
  listText: {
    fontSize: 18,
    alignItems: "center",
    margin: 10,
    width: 280,
  },
  fotter: {
    flexDirection: "column",
    alignItems: "center",
    marginBottom: 25,
    width: 280,
    justifyContent: "space-between",
  },
  textContainer: {
    flex: 1,
    flexDirection: "column",
    fontSize: 18,
  },
  app: {
    flex: 2,
    flexDirection: "row",
    flexWrap: "wrap",
    backgroundColor: "rgb(250,250,250)",
    padding: 2,
  },
  item: {
    flex: 18,
    padding: 8,
    margin: 1,
    borderRadius: 4,
    height: "60%",
  },
  bleContainer: {
    flexDirection: "row",
    flexWrap: "wrap",
    width: "70%",
    padding: 10,
    alignItems: "center",
    justifyContent: "space-between",
  },
  button: {
    alignItems: "center",
    justifyContent: "center",
    width: 100,
    height: 35,
    borderRadius: 4,
    backgroundColor: "rgb(69,78,105)",
    elevation: 5,
  },
  textButton: {
    fontSize: 14,
    letterSpacing: 0.35,
    color: "white",
  },
});

export default styles;
