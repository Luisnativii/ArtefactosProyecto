function DashboardElement({ title, value, unit }) {
  return (
    <div className="p-4 rounded-md bg-slate-50 text-neutral-800 flex flex-col gap-4 justify-center items-center">
      <h2 className="font-bold text-lg">{title}</h2>
      <p className="text-2xl">{value ? value : "Sin datos"}</p>
      {/* Si quieres agregar una unidad de medida */}
      {unit && <p className="text-sm">{unit}</p>}
    </div>
  );
}

export default DashboardElement;
