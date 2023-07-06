import { StyleSheet } from "react-native";

// styles object
const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "flex-start",
  },
  title: {
    fontSize: 20,
    paddingTop: 25,
  },
  bleContainer: {
    paddingTop: 30,
    alignItems: "center",
    justifyContent: "flex-start",
  },
  textBox: {
    alignItems: "center",
    marginVertical: 10,
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
    flex: 1,
    flexDirection: "row",
    alignItems: "flex-end",
    marginBottom: 50,
    width: 280,
    justifyContent: "space-between",
  },
  textContainer: {
    flex: 1,
    flexDirection: "column",
    fontSize: 20,
  },
  app: {
    flex: 1,
    flexDirection: "row",
    flexWrap: "wrap",
    backgroundColor: "rgb(250,250,250)",
    padding: 2,
  },
  item: {
    flex: 16,
    padding: 8,
    margin: 1,
    borderRadius: 4,
    height: "50%",
  },
});

export default styles;
